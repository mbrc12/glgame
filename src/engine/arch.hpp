#pragma once

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <string>

namespace Engine {

class Node {
  public:
    virtual ~Node() = default;

    std::string getPath() const { 
        using namespace std::ranges;
        return path | views::join_with('/') | to<std::string>();
    }

    virtual void preUpdate(float deltaTime) {}

    virtual void update(float deltaTime) {
        preUpdate(deltaTime);

        for (auto& child : children) {
            child.second->update(deltaTime);
        }

        postUpdate(deltaTime);
    }

    virtual void postUpdate(float deltaTime) {}

    template <typename T> void add(const std::string& name, T&& child) {
        auto it = children.find(name);
        assert(it == children.end() && "Child already exists");

        auto child_ptr = std::make_shared<T>(child);
        child_ptr->root = root;

        children[name] = child_ptr;
    }

    void remove(const std::string& name) {
        auto it = children.find(name);
        assert(it != children.end() && "Child does not exist");

        children.erase(it);
    }

    std::optional<std::shared_ptr<Node>> getChild(const std::string& name) {
        auto it = children.find(name);
        if (it != children.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void on(const std::string& name, std::function<void(void*)> callback) {
        auto it = callbacks.find(name);
        assert(it == callbacks.end() && "Callback already exists");

        callbacks[name] = callback;
    }

    void trigger(const std::string& name, void* data, const std::string& relative_path = "..") {
        auto it = callbacks.find(name);
        assert(it != callbacks.end() && "Callback does not exist");

        it->second(data);
    }

  private:
    std::shared_ptr<Node> root;
    std::vector<std::string> path; 
    std::map<std::string, std::shared_ptr<Node>> children;
    std::map<std::string, std::function<void(void*)>> callbacks;
};

}; // namespace Engine
