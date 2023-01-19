{
  "targets": [
    {
      "target_name": "oodle",
      "sources": ["main.cc"],
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "conditions": [
        ["OS=='win'", {
          "libraries": ["<(module_root_dir)/lib/oo2net_win64.lib"],
        }],
        ["OS=='linux' and target_arch=='arm64'", {
          "libraries": ["<(module_root_dir)/lib/liboo2netlinuxarm64.a"],
        }],
        ["OS=='linux'", {
          "libraries": ["<(module_root_dir)/lib/liboo2netlinux64.a"],
        }]
      ],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"]
    }
  ]
}
