# AutoBroadcast 
[![Downloads](https://endgit.dev/shield.dl.total/autobroadcast)](https://endgit.dev/plugins/autobroadcast)

An [Endstone](https://endstone.dev) plugin that periodically broadcasts configurable messages to all online players.

## Features
- Cycle through a list of messages at a configurable interval.
- Choose between **sequential** (ordered) or **random** broadcast mode.
- Optional colour-coded prefix applied to every message.
- Reload configuration live with `/autobroadcast reload` — no server restart required.
- Zero console spam on startup/shutdown.

## Installation

1. Build the plugin with CMake (see [Building](#building)).
2. Copy the resulting `AutoBroadcast.dll` / `libAutoBroadcast.so` into the server's `plugins/` folder.
3. Start (or restart) the server.  
   A default `config.yml` is created automatically in `plugins/AutoBroadcast/`.

## Configuration

`plugins/AutoBroadcast/config.yml` is created on first start with sensible defaults:

```yaml
# How many seconds between each broadcast message. Minimum: 1
interval: 60

# Message order: sequential (default) or random
order: sequential

# Prefix prepended to every message. Supports § colour codes. Leave empty to disable.
prefix: "§6[Info] §r"

# Messages to cycle through. § colour codes are supported.
messages:
    - "§aWelcome to the server! Type /help for a list of commands."
    - "§ePlease respect all players and follow the server rules."
    - "§bHaving trouble? Ask a staff member for help!"
```

### Options

| Key        | Type    | Default        | Description                                                         |
| ---------- | ------- | -------------- | ------------------------------------------------------------------- |
| `interval` | integer | `60`           | Seconds between broadcasts (minimum 1).                             |
| `order`    | string  | `sequential`   | `sequential` — messages in order; `random` — random pick each time. |
| `prefix`   | string  | `§6[Info] §r`  | Prepended to every message. Set to `""` to disable.                 |
| `messages` | list    | _(3 examples)_ | List of strings to broadcast.                                       |

### Colour codes

Minecraft colour codes (`§a`, `§b`, …) work in both `prefix` and `messages`.

## Commands

| Command                 | Description                             | Permission                     |
| ----------------------- | --------------------------------------- | ------------------------------ |
| `/autobroadcast reload` | Reload `config.yml` without restarting. | `autobroadcast.command.reload` |

## Permissions

| Permission                     | Default  | Description                             |
| ------------------------------ | -------- | --------------------------------------- |
| `autobroadcast.command.reload` | Operator | Allows running `/autobroadcast reload`. |

## Building

```bash
cmake -B build -S .
cmake --build build --config Release
```

The plugin binary will be in `build/` (exact path depends on platform).

## License

MIT — see [LICENSE](LICENSE).
