import osmnx as ox
import networkx as nx
import pandas as pd

# 1. Define the Place
place_name = "Karachi, Pakistan"
print(f"1. Fetching road network for '{place_name}'...")

# Download the road network
G = ox.graph_from_place(place_name, network_type='drive')

# Convert to simple integer IDs (0, 1, 2...)
G_simple = nx.Graph(G)
G_int = nx.convert_node_labels_to_integers(G_simple, first_label=0)

print(f"   Graph ready: {G_int.number_of_nodes()} nodes, {G_int.number_of_edges()} edges.")

# --- 2. Save Network Files ---
print("2. Writing locations and roads files...")

with open('../text_files/karachi_locations.txt', 'w', encoding='utf-8') as f:
    for node in G_int.nodes():
        f.write(f"{node}\n")

with open('../text_files/karachi_roads.txt', 'w', encoding='utf-8') as f:
    for u, v, data in G_int.edges(data=True):
        distance = int(round(data.get('length', 1)))
        f.write(f"{u} {v} {distance}\n")

# --- 3. Fetch Landmarks (Optimized) ---
print("3. Auto-discovering landmarks (Vectorized Mode)...")

tags = {
    'amenity': ['university', 'hospital', 'cinema', 'marketplace', 'bus_station', 'mall', 'police', 'fire_station', 'fuel', 'bank'],
    'tourism': ['attraction', 'hotel', 'museum', 'theme_park'],
    'shop': ['mall', 'supermarket'],
    'historic': ['monument', 'memorial'],
    'building': ['train_station'],
    'highway': ['motorway', 'trunk', 'primary'] 
}

try:
    # Fetch all features at once
    pois = ox.features_from_place(place_name, tags)
    pois = pois[pois['name'].notnull()]
    
    print(f"   Found {len(pois)} items. Calculating nearest nodes in BULK...")

    # --- OPTIMIZATION START ---
    # Instead of a loop, we extract all coordinates first
    poi_names = []
    poi_lons = []
    poi_lats = []

    for index, row in pois.iterrows():
        name = row['name']
        clean_name = name.replace(" ", "_").replace("'", "").replace("-", "_").replace(".", "").replace("/", "_")
        
        if row.geometry.geom_type == 'Point':
            lat = row.geometry.y
            lon = row.geometry.x
        else:
            lat = row.geometry.centroid.y
            lon = row.geometry.centroid.x
            
        poi_names.append(clean_name)
        poi_lons.append(lon)
        poi_lats.append(lat)

    # THE MAGIC LINE: Find all 20,000+ nearest nodes in ONE go
    # (This is 100x faster than the loop)
    nearest_nodes = ox.distance.nearest_nodes(G_int, X=poi_lons, Y=poi_lats)

    # Write results
    print("   Writing landmarks file...")
    unique_landmarks = set()
    
    with open('../text_files/karachi_landmarks.txt', 'w', encoding='utf-8') as f:
        for name, node_id in zip(poi_names, nearest_nodes):
            if name not in unique_landmarks:
                f.write(f"{name} {node_id}\n")
                unique_landmarks.add(name)
    # --- OPTIMIZATION END ---

    print(f"   Success! Saved {len(unique_landmarks)} unique landmarks.")

except Exception as e:
    print(f"Error: {e}")

print("All done!")