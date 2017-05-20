{
    "targets": [{
      "target_name": "recommender",
      "sources": [
        "recommender_node.cpp",
        "src/recommender.cpp",
        "src/Utils.cpp"
      ],
      "cflags": ["-Wall", "-std=c++11"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "conditions": [ 
        [ "OS=='mac'", { 
            "xcode_settings": { 
                "OTHER_CPLUSPLUSFLAGS" : ["-std=c++11","-stdlib=libc++"], 
                "OTHER_LDFLAGS": ["-stdlib=libc++"], 
                "MACOSX_DEPLOYMENT_TARGET": "10.7" } 
            }
        ] 
      ] 
    }]
}