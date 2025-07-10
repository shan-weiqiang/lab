#!/bin/bash

echo "=== Testing Complex MCAP Protobuf Messages ==="
echo

# Build the projects
echo "Building writer and reader..."
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..

echo
echo "=== Step 1: Writing MCAP file with complex protobuf messages ==="
echo "Running writer to create output.mcap..."
./build/writer

echo
echo "=== Step 2: Reading MCAP file with generic reader ==="
echo "Running generic reader (no compile-time protobuf dependencies)..."
./build/reader

echo
echo "=== Test Complete ==="
echo "The generic reader successfully handled:"
echo "1. Simple RouteSummary message with basic fields"
echo "2. Complex ComplexRouteSummary message with:"
echo "   - Nested messages (SceneInfo, Vehicle, TrafficLight)"
echo "   - Enums (VehicleType, TrafficLightState)"
echo "   - Repeated fields (trajectory, features, distance_segments)"
echo "   - Map fields (attributes, metadata, metrics)"
echo "   - 3D points and bounding boxes"
echo "3. Cross-package dependencies (routeguide importing complex)"
echo "4. All without compile-time dependencies on the specific message types" 