// Copyright (c) 2026, harryitz. All Rights Reserved.
// SPDX-License-Identifier: MIT

/// @file plugin.cpp
/// AutoBroadcast — periodically broadcasts configurable messages to all players.

#include "plugin.h"

#include <filesystem>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>

#include <endstone/endstone.hpp>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

namespace harryitz::auto_broadcast {

namespace fs = std::filesystem;

// ─────────────────────────────────────────────────────────────────────────────
// Plugin metadata
// ─────────────────────────────────────────────────────────────────────────────

ENDSTONE_PLUGIN("AutoBroadcast", "1.0.0", AutoBroadcastPlugin)
{
    description = "Periodically broadcasts configurable messages to all online players.";
    authors     = {"harryitz"};
    website     = "";
    prefix      = "AutoBroadcast";

    command("autobroadcast")
        .description("AutoBroadcast management command.")
        .usages("/autobroadcast reload")
        .permissions("autobroadcast.command.reload");

    permission("autobroadcast.command.reload")
        .description("Allows reloading the AutoBroadcast configuration.")
        .default_(endstone::PermissionDefault::Operator);
}

// ─────────────────────────────────────────────────────────────────────────────
// Default config text written on first run
// ─────────────────────────────────────────────────────────────────────────────

static constexpr const char *kDefaultConfig =
    "# AutoBroadcast configuration\n"
    "#\n"
    "# interval: seconds between each broadcast message (minimum: 1)\n"
    "interval: 60\n"
    "\n"
    "# order: sequential (default) or random\n"
    "order: sequential\n"
    "\n"
    "# prefix: prepended to every message; supports § colour codes.\n"
    "# Leave empty to disable.\n"
    "prefix: \"\\u00a76[Info] \\u00a7r\"\n"
    "\n"
    "# messages: list of strings to broadcast in rotation.\n"
    "messages:\n"
    "  - \"Welcome to the server! Type /help for a list of commands.\"\n"
    "  - \"Please respect all players and follow the server rules.\"\n"
    "  - \"Having trouble? Ask a staff member for help!\"\n";

// ─────────────────────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────────────────────

void AutoBroadcastPlugin::onEnable()
{
    // Ensure the data directory exists.
    const auto data_dir = getDataFolder();
    if (!fs::exists(data_dir)) {
        fs::create_directories(data_dir);
    }

    // Write default config if it does not exist yet.
    const auto config_path = data_dir / "config.yml";
    if (!fs::exists(config_path)) {
        std::ofstream out{config_path};
        out << kDefaultConfig;
    }

    loadConfig();
    scheduleTask();
}

void AutoBroadcastPlugin::onDisable()
{
    cancelTask();
}

// ─────────────────────────────────────────────────────────────────────────────
// Command handling
// ─────────────────────────────────────────────────────────────────────────────

bool AutoBroadcastPlugin::onCommand(endstone::CommandSender &sender, const endstone::Command & /*command*/,
                                    const std::vector<std::string> &args)
{
    if (args.empty() || args[0] != "reload") {
        sender.sendMessage("§cUsage: /autobroadcast reload");
        return true;
    }

    cancelTask();
    loadConfig();
    scheduleTask();
    sender.sendMessage("§aAutoBroadcast configuration reloaded.");
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Config loading
// ─────────────────────────────────────────────────────────────────────────────

void AutoBroadcastPlugin::loadConfig()
{
    const auto config_path = getDataFolder() / "config.yml";

    BroadcastConfig cfg;

    try {
        const YAML::Node root = YAML::LoadFile(config_path.string());

        // interval (seconds → ticks; clamp to at least 20 ticks = 1 second)
        const int interval_sec = root["interval"] ? root["interval"].as<int>(60) : 60;
        cfg.interval_ticks = static_cast<std::uint64_t>(std::max(1, interval_sec)) * 20;

        // order
        const std::string order = root["order"] ? root["order"].as<std::string>("sequential") : "sequential";
        cfg.random_order = (order == "random");

        // prefix
        cfg.prefix = root["prefix"] ? root["prefix"].as<std::string>("") : "";

        // messages
        if (root["messages"] && root["messages"].IsSequence()) {
            for (const auto &node : root["messages"]) {
                if (node.IsScalar()) {
                    cfg.messages.push_back(node.as<std::string>());
                }
            }
        }
    }
    catch (const YAML::Exception &ex) {
        getLogger().error("Failed to load config.yml: {}", ex.what());
    }

    config_   = std::move(cfg);
    next_index_ = 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Scheduler helpers
// ─────────────────────────────────────────────────────────────────────────────

void AutoBroadcastPlugin::scheduleTask()
{
    if (config_.messages.empty()) {
        getLogger().warning("No messages configured — broadcasts are disabled.");
        return;
    }

    broadcast_task_ = getServer().getScheduler().runTaskTimer(
        *this,
        [this]() { broadcastNext(); },
        config_.interval_ticks,   // initial delay
        config_.interval_ticks    // repeat period
    );
}

void AutoBroadcastPlugin::cancelTask()
{
    if (broadcast_task_ && !broadcast_task_->isCancelled()) {
        broadcast_task_->cancel();
    }
    broadcast_task_.reset();
}

// ─────────────────────────────────────────────────────────────────────────────
// Broadcast logic
// ─────────────────────────────────────────────────────────────────────────────

void AutoBroadcastPlugin::broadcastNext()
{
    if (config_.messages.empty()) {
        return;
    }

    std::size_t idx = 0;
    if (config_.random_order) {
        // Thread-local RNG — avoids contention on the main thread.
        static thread_local std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<std::size_t> dist{0, config_.messages.size() - 1};
        idx = dist(rng);
    }
    else {
        idx       = next_index_ % config_.messages.size();
        next_index_ = idx + 1;
    }

    const std::string &text = config_.messages[idx];
    const std::string  full = config_.prefix + text;
    getServer().broadcastMessage(full);
}

}  // namespace harryitz::auto_broadcast
