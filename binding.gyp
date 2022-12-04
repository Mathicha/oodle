{
  "targets": [
    {
      "target_name": "oodle",
      "sources": ["main.cc"],
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "libraries": ["<(module_root_dir)/oo2net_win64.lib"],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"]
    }
  ]
}
