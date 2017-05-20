{
    "targets": [{
        "target_name": "recommender",
      "sources": [
        "recommender_node.cpp",
        "src/recommender.cpp",
        "src/Utils.cpp"
      ],
        "include_dirs": [
            "<!(node -e \"require('nan')\")"
        ]
    }]
}