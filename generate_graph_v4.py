import osmnx as ox
import networkx as nx

# 1. Define the Place
place_name = "Karachi, Pakistan"
print(f"1. Fetching road network for '{place_name}'...")

# Download the road network
G = ox.graph_from_place(place_name, network_type='drive')

# Convert to simple integer IDs (0, 1, 2...) for C++ compatibility
G_simple = nx.Graph(G)
G_int = nx.convert_node_labels_to_integers(G_simple, first_label=0)

print(f"   Graph ready: {G_int.number_of_nodes()} nodes, {G_int.number_of_edges()} edges.")

# --- 2. Save Network Files ---
print("2. Writing karachi_locations.txt and karachi_roads.txt...")

with open('karachi_locations.txt', 'w', encoding='utf-8') as f:
    for node in G_int.nodes():
        f.write(f"{node}\n")

with open('karachi_roads.txt', 'w', encoding='utf-8') as f:
    for u, v, data in G_int.edges(data=True):
        distance = int(round(data.get('length', 1)))
        f.write(f"{u} {v} {distance}\n")

# --- 3. Fetch Landmarks Automatically ---
print("3. Auto-discovering landmarks (this may take 1-2 minutes)...")

# === V4 TAGS: Major Landmarks & Main Roads Only ===
tags = {
    'amenity': [
        'university', 'hospital', 'cinema', 'marketplace', 'bus_station', 'mall',
        'police', 'fire_station', 'fuel', 'restaurant', 'pharmacy', 'bank'
    ],
    'tourism': ['attraction', 'hotel', 'museum', 'theme_park'],
    'shop': ['mall', 'supermarket', 'electronics'],
    'historic': ['monument', 'memorial'],
    'building': ['train_station'],
    # Major Roads only (No residential streets)
    'highway': ['motorway', 'trunk', 'primary'] 
}

# Fetch the features
try:
    pois = ox.features_from_place(place_name, tags)
    
    # Filter: Must have a name
    pois = pois[pois['name'].notnull()]
    
    print(f"   Found {len(pois)} potential landmarks. Processing...")

    landmarks = {}
    
    with open('karachi_landmarks.txt', 'w', encoding='utf-8') as f:
        count = 0
        for index, row in pois.iterrows():
            try:
                name = row['name']
                
                # Get coordinates (handles Points, Polygons, and Lines)
                if row.geometry.geom_type == 'Point':
                    lat = row.geometry.y
                    lon = row.geometry.x
                else:
                    lat = row.geometry.centroid.y
                    lon = row.geometry.centroid.x
                
                # Find nearest node in the integer graph
                nearest_int_id = ox.distance.nearest_nodes(G_int, X=lon, Y=lat)
                
                # Clean the name (replace spaces/symbols for C++)
                clean_name = name.replace(" ", "_").replace("'", "").replace("-", "_").replace(".", "").replace("/", "_")
                
                # Save unique names only
                if clean_name not in landmarks:
                    landmarks[clean_name] = nearest_int_id
                    f.write(f"{clean_name} {nearest_int_id}\n")
                    count += 1
                    
            except Exception as e:
                continue

    print(f"   Success! Saved {count} unique landmarks to 'karachi_landmarks.txt'.")

except Exception as e:
    print(f"Error fetching landmarks: {e}")

print("All done. You are ready to compile C++.")