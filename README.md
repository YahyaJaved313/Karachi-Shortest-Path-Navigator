# Karachi Shortest Path Navigator

A high-performance, console-based navigation system for **Karachi, Pakistan**, built using **C++**. This application utilizes **Dijkstra’s Algorithm** to calculate the shortest paths between thousands of real-world landmarks, road intersections, and essential services based on OpenStreetMap (OSM) data.

## 🚀 Features

  * **Shortest Route Finder**: Calculate the most efficient path between two locations using real-time graph traversal.
  * **Emergency Services Locator**: Instantly find the nearest **Police Station**, **Hospital**, or **Fire Station** from your current location.
  * **Public Transport Finder**: Locate the closest **Bus Station**.
  * **Fuel Station Locator**: Find the nearest **Petrol Pump**.
  * **Landmark Browser**: Browse a repository of mapped locations within Karachi.
  * **Turn-by-Turn Navigation**: View detailed or simplified route instructions.
  * **Real-World Data**: Uses road networks and points of interest extracted directly from OpenStreetMap.

-----

## 🛠️ Tech Stack

  * **Core Logic**: C++ (STL, Custom Graph Structures)
  * **Algorithm**: Dijkstra’s Shortest Path Algorithm (Priority Queue optimized)
  * **Data Extraction**: Python (OSMnx, NetworkX, Pandas)
  * **Data Source**: OpenStreetMap (OSM)

-----

## 📂 Project Structure

```text
.
├── pythonScript/
│   └── generate_graph_fast.py    # Script to fetch/process OSM data
├── src/
│   ├── main.cpp                  # Entry point (Menu & UI)
│   ├── dijkstra.cpp/.h           # Pathfinding algorithm implementation
│   ├── graph.cpp/.h              # Graph creation & loading logic
│   ├── features.cpp/.h           # Service finders & route printing
│   ├── structures.h              # Node & Edge class definitions
│   ├── utils.cpp/.h              # Helper functions (string normalization, etc.)
│   └── globals.cpp/.h            # Global variables (graph nodes, lookups)
├── text_files/                   # Generated data files (Required for C++ app)
│   ├── karachi_locations.txt     # Node IDs
│   ├── karachi_roads.txt         # Edge connections & weights
│   └── karachi_landmarks.txt     # Names mapped to Node IDs
└── README.md
```

-----

## ⚙️ Setup & Installation

### 1\. Prerequisites

Ensure you have the following installed:

  * **G++ Compiler** (MinGW for Windows, GCC for Linux/macOS)
  * **Python 3.x** (for generating map data)

### 2\. Generate Map Data

Before running the C++ application, you must generate the graph data files. The provided Python script fetches the latest road network for Karachi.

1.  Navigate to the `pythonScript` folder:
    ```bash
    cd pythonScript
    ```
2.  Install the required Python libraries:
    ```bash
    pip install osmnx networkx pandas
    ```
3.  Run the generation script:
    ```bash
    python generate_graph_fast.py
    ```
    *This will create the `karachi_locations.txt`, `karachi_roads.txt`, and `karachi_landmarks.txt` files inside the `text_files/` directory.*

### 3\. Compile the C++ Application

**Important:** This project uses a **Unity Build** approach where `.cpp` files are included directly into `main.cpp`. Do **not** compile the files individually.

1.  Navigate to the `src` folder:
    ```bash
    cd ../src
    ```
2.  Compile using `g++`:
    ```bash
    g++ main.cpp -o navigator
    ```
    *(If you are on Linux/macOS, use `-o navigator` instead of `navigator.exe`)*

### 4\. Run the Navigator

```bash
./navigator
```

-----

## 📖 Usage Guide

Once the application is running, you will see the **Main Menu**:

### **[1] Calculate Shortest Route**

  * Enter the **Start Location** (e.g., *"Nust University"*).
  * Enter the **Destination** (e.g., *"Dolmen Mall"*).
  * Choose your view preference:
      * **[D] Detailed**: Shows every intersection node ID along the path.
      * **[L] Landmarks Only**: Shows a clean view containing only named roads/places.

### **[3] Emergency Services**

  * Select the service type (Police, Hospital, Fire).
  * Enter your current location.
  * The system will find the **nearest** facility and provide a route to it.

### **[5] Fuel Station**

  * Enter your location to find the closest petrol pump.

-----

## 🧠 How It Works

1.  **Graph Construction**:
      * The **Nodes** represent intersections or specific locations on the map.
      * The **Edges** represent roads connecting these nodes, weighted by distance (meters).
2.  **Data Loading**:
      * On startup, the app reads thousands of nodes and edges from the `text_files` directory into memory.
3.  **Pathfinding**:
      * When a user requests a route, **Dijkstra's Algorithm** traverses the graph from the starting node, exploring neighbors and updating the shortest known distance to each until the destination is reached.
4.  **Nearest Neighbor Search**:
      * For services (e.g., "Find nearest Hospital"), the algorithm runs from the user's location and stops as soon as it encounters a node tagged with the requested service type.

-----

## ⚠️ Troubleshooting

  * **"Error: Could not open ../text\_files/..."**:
      * Ensure you ran the Python script (`generate_graph_fast.py`) successfully.
      * Ensure the `text_files` folder exists in the project root (one level above `src`).
  * **"Multiple definition of..." linker errors**:
      * You likely tried to compile all files (e.g., `g++ *.cpp`). Only compile `main.cpp` because it already includes the other implementation files.

-----

## 📜 License

This project is open-source and free to use for educational and non-commercial purposes.
