{
    "targets": [{
        "target_name": "recommender",
        "sources": ["recommender_node.cpp", "include/recommender.h", "src/recommender.cpp"],
        "include_dirs": [
            "<!(node -e \"require('nan')\")"
        ]
    }]
}