#pragma once

namespace json_samples {
    constexpr const char* creature_template = R"json(
{
    "name": "TROLL",
    "speed": 0.75,
    "components": [
        {
            "type": "Render",
            "glyph": 84,
            "color": "#FF0000"
        },
        {
            "type": "Vision",
            "radius": 10
        },
        {
            "type": "Life"
        },
        {
            "type": "Behavior",
            "root": {
                "type": "Condition",
                "condition": "IsAlive",
                "child": {
                    "type": "Sequence",
                    "children": [
                        {
                            "type": "ClosestEntity",
                            "found_target_key": "walk_target"
                        },
                        {
                            "type": "Walk",
                            "walk_target_key": "walk_target"
                        }
                    ]
                }
            }
        }
    ]
}
)json";

    constexpr const char* app_state = R"json(
{
  "menu": {
    "id": "file",
    "value": "File",
    "popup": {
      "menuitem": [
        {"value": "New", "onclick": "CreateNewDoc()"},
        {"value": "Open", "onclick": "OpenDoc()"},
        {"value": "Close", "onclick": "CloseDoc()"}
      ]
    }
  },
  "window": {
    "title": "Sample Widget",
    "position": {
      "x": 200,
      "y": 300,
      "units": "pixels"
    },
    "dimensions": {
      "width": "400px",
      "height": "500px"
    },
    "visible": true
  },
  "documents": [
    {
      "type": "text",
      "content": "Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
      "metadata": {
        "author": "John Doe",
        "tags": ["example", "text"],
        "lastEdited": null,
        "published": false
      }
    },
    {
      "type": "spreadsheet",
      "content": null,
      "metadata": {
        "author": "Jane Doe",
        "tags": ["data", "spreadsheet"],
        "lastEdited": "2023-10-11T14:48:00Z",
        "published": true
      }
    }
  ],
  "settings": {
    "theme": "dark",
    "autoSave": {
      "enabled": true,
      "interval": 10
    },
    "languages": ["en", "es", "fr", {"code": "de", "displayName": "German"}],
    "shortcuts": {
      "copy": "Ctrl+C",
      "paste": "Ctrl+V",
      "custom": [
        {"action": "bold", "keys": ["Ctrl", "B"]},
        {"action": "italic", "keys": ["Ctrl", "I"]},
        {"action": "save", "keys": []}
      ]
    }
  },
  "users": [
    {
      "name": "admin",
      "roles": ["administrator", "editor"],
      "active": true,
      "permissions": {
        "canEdit": true,
        "canDelete": true,
        "accessLevels": [1, 2, 3]
      }
    },
    {
      "name": "guest",
      "roles": ["viewer"],
      "active": false,
      "permissions": {
        "canEdit": false,
        "canDelete": false,
        "accessLevels": []
      }
    }
  ],
  "history": [
    {
      "action": "edit",
      "timestamp": "2024-01-12T09:15:00Z",
      "user": "admin"
    },
    {
      "action": "login",
      "timestamp": "2024-01-11T08:30:00Z",
      "user": "guest",
      "details": {
        "success": true,
        "ip": "192.168.1.1"
      }
    }
  ]
}
)json";

} // namespace json_samples
