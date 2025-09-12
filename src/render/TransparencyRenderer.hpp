#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TransparencyRenderer {
public:
    struct TransparentObject {
        glm::vec3 position;
        glm::vec3 bounds_min, bounds_max; // AABB for intersection tests
        float alpha;
        int mesh_id;
        glm::mat4 model_matrix;
        
        TransparentObject(glm::vec3 pos, glm::vec3 min_bounds, glm::vec3 max_bounds, 
                         float a, int id, glm::mat4 model = glm::mat4(1.0f))
            : position(pos), bounds_min(min_bounds), bounds_max(max_bounds),
              alpha(a), mesh_id(id), model_matrix(model) {}
    };

private:
    struct DependencyGraph {
        std::unordered_map<int, std::unordered_set<int>> adjacency_list;
        std::unordered_set<int> nodes;
        
        void add_edge(int from, int to) {
            adjacency_list[from].insert(to);
            nodes.insert(from);
            nodes.insert(to);
        }
        
        void clear() {
            adjacency_list.clear();
            nodes.clear();
        }
    };
    
    std::vector<TransparentObject> objects;
    DependencyGraph dependency_graph;
    std::vector<int> render_order;
    glm::vec3 last_camera_pos;
    bool needs_resort = true;
    
    // Frustum culling optimization (optional)
    struct Frustum {
        glm::vec4 planes[6];
    };
    
public:
    TransparencyRenderer() {
        objects.reserve(1000); // Pre-allocate for performance
    }
    
    void add_object(const TransparentObject& obj) {
        objects.push_back(obj);
        needs_resort = true;
    }
    
    void clear_objects() {
        objects.clear();
        needs_resort = true;
    }
    
    // The meat and potatoes - build dependency graph
    void build_dependency_graph(const glm::vec3& camera_pos, const glm::vec3& camera_dir) {
        dependency_graph.clear();
        
        for (size_t i = 0; i < objects.size(); i++) {
            for (size_t j = i + 1; j < objects.size(); j++) {
                if (i == j) continue;
                
                // Check if objects actually intersect in view space
                if (!objects_potentially_overlap(objects[i], objects[j], camera_pos, camera_dir)) {
                    continue;
                }
                
                // Determine rendering order based on depth and occlusion
                float depth_i = glm::dot(objects[i].position - camera_pos, camera_dir);
                float depth_j = glm::dot(objects[j].position - camera_pos, camera_dir);
                
                if (depth_i > depth_j + 0.001f) { // i is farther, should render first
                    dependency_graph.add_edge(i, j);
                } else if (depth_j > depth_i + 0.001f) { // j is farther, should render first
                    dependency_graph.add_edge(j, i);
                }
                // If depths are equal, no dependency needed
            }
        }
    }
    
    // DFS-based topological sort
    std::vector<int> topological_sort() {
        std::vector<int> result;
        std::unordered_set<int> visited;
        std::unordered_set<int> recursion_stack; // Cycle detection
        
        for (int node : dependency_graph.nodes) {
            if (visited.find(node) == visited.end()) {
                if (!dfs_visit(node, visited, recursion_stack, result)) {
                    // Cycle detected - fallback to simple distance sort
                    return fallback_distance_sort(last_camera_pos);
                }
            }
        }
        
        std::reverse(result.begin(), result.end());
        return result;
    }
    
    // Main update function - call this each frame
    void update(const glm::vec3& camera_pos, const glm::vec3& camera_dir) {
        // Only resort if camera moved significantly
        if (needs_resort || glm::distance(camera_pos, last_camera_pos) > 0.1f) {
            build_dependency_graph(camera_pos, camera_dir);
            render_order = topological_sort();
            last_camera_pos = camera_pos;
            needs_resort = false;
        }
    }
    
    // Get sorted objects for rendering
    const std::vector<int>& get_render_order() const {
        return render_order;
    }
    
    const TransparentObject& get_object(int index) const {
        return objects[index];
    }
    
    size_t object_count() const {
        return objects.size();
    }

private:
    bool dfs_visit(int node, std::unordered_set<int>& visited, 
                   std::unordered_set<int>& recursion_stack, 
                   std::vector<int>& result) {
        visited.insert(node);
        recursion_stack.insert(node);
        
        auto it = dependency_graph.adjacency_list.find(node);
        if (it != dependency_graph.adjacency_list.end()) {
            for (int neighbor : it->second) {
                if (recursion_stack.find(neighbor) != recursion_stack.end()) {
                    return false; // Cycle detected
                }
                if (visited.find(neighbor) == visited.end()) {
                    if (!dfs_visit(neighbor, visited, recursion_stack, result)) {
                        return false;
                    }
                }
            }
        }
        
        recursion_stack.erase(node);
        result.push_back(node);
        return true;
    }
    
    // Check if two objects could potentially overlap from camera's perspective
    bool objects_potentially_overlap(const TransparentObject& a, const TransparentObject& b,
                                   const glm::vec3& camera_pos, const glm::vec3& camera_dir) {
        // Simple AABB intersection test in world space
        return !(a.bounds_max.x < b.bounds_min.x || a.bounds_min.x > b.bounds_max.x ||
                 a.bounds_max.y < b.bounds_min.y || a.bounds_min.y > b.bounds_max.y ||
                 a.bounds_max.z < b.bounds_min.z || a.bounds_min.z > b.bounds_max.z);
    }
    
    // Fallback when cycles are detected
    std::vector<int> fallback_distance_sort(const glm::vec3& camera_pos) {
        std::vector<std::pair<float, int>> distances;
        
        for (size_t i = 0; i < objects.size(); i++) {
            float dist = glm::length2(objects[i].position - camera_pos);
            distances.emplace_back(dist, static_cast<int>(i));
        }
        
        std::sort(distances.begin(), distances.end(), 
                 [](const auto& a, const auto& b) { return a.first > b.first; });
        
        std::vector<int> result;
        for (const auto& pair : distances) {
            result.push_back(pair.second);
        }
        
        return result;
    }
};

// Usage example
class Scene {
private:
    TransparencyRenderer transparency_renderer;
    
public:
    void setup_transparent_objects() {
        // Add some transparent windows
        transparency_renderer.add_object({
            glm::vec3(0, 0, 0),              // position
            glm::vec3(-0.5f, -0.5f, -0.1f),  // bounds min
            glm::vec3(0.5f, 0.5f, 0.1f),     // bounds max
            0.5f,                            // alpha
            0                                // mesh id
        });
        
        transparency_renderer.add_object({
            glm::vec3(1, 0, -1),
            glm::vec3(0.5f, -0.5f, -1.1f),
            glm::vec3(1.5f, 0.5f, -0.9f),
            0.7f,
            1
        });
    }
    
    void render_frame(const glm::vec3& camera_pos, const glm::vec3& camera_dir) {
        // Update transparency sorting
        transparency_renderer.update(camera_pos, camera_dir);
        
        // Render opaque objects first...
        render_opaque_objects();
        
        // Then render transparent objects in sorted order
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE); // Don't write to depth buffer
        
        const auto& render_order = transparency_renderer.get_render_order();
        for (int index : render_order) {
            const auto& obj = transparency_renderer.get_object(index);
            render_transparent_object(obj);
        }
        
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    
    void render_opaque_objects() { /* ... */ }
    void render_transparent_object(const TransparencyRenderer::TransparentObject& obj) { /* ... */ }
};