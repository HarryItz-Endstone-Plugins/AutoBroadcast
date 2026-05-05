// Copyright (c) 2026, harryitz. All Rights Reserved.
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <endstone/command/command.h>
#include <endstone/command/command_sender.h>
#include <endstone/plugin/plugin.h>
#include <endstone/scheduler/task.h>

namespace harryitz::auto_broadcast {

struct BroadcastConfig {
    // Interval between messages in server ticks (20 ticks = 1 second).
    std::uint64_t interval_ticks{1200};
    // When true, messages are sent in random order.
    bool random_order{false};
    // Optional prefix prepended to every message (supports § colour codes).
    std::string prefix;
    // The messages to cycle through.
    std::vector<std::string> messages;
};

class AutoBroadcastPlugin final : public endstone::Plugin {
public:
    void onEnable() override;
    void onDisable() override;

    bool onCommand(endstone::CommandSender &sender, const endstone::Command &command,
                   const std::vector<std::string> &args) override;

private:
    // Load (or reload) config.yml from the plugin data folder.
    void loadConfig();
    // Cancel any existing repeating task and schedule a new one.
    void scheduleTask();
    // Cancel the active repeating task, if any.
    void cancelTask();
    // Broadcast the next message according to current config.
    void broadcastNext();

    BroadcastConfig config_;
    // Index of the next message to send (used in sequential mode).
    std::size_t next_index_{0};
    // Active repeating scheduler task (nullptr when not scheduled).
    std::shared_ptr<endstone::Task> broadcast_task_;
};

}  // namespace harryitz::auto_broadcast
