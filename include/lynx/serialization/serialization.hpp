#pragma once
#ifdef KIT_USE_YAML_CPP

#include "kit/serialization/yaml/codec.hpp"
#include "kit/serialization/yaml/glm.hpp"
#include "lynx/app/app.hpp"

template <lynx::Dimension Dim> struct kit::yaml::codec<lynx::layer<Dim>>
{
    static YAML::Node encode(const lynx::layer<Dim> &layer)
    {
        YAML::Node node;
        node["Enabled"] = layer.enabled;
        return node;
    }
    static bool decode(const YAML::Node &node, lynx::layer<Dim> &layer)
    {
        if (!node.IsMap())
            return false;
        layer.enabled = node["Enabled"].as<bool>();

        return true;
    }
};

template <lynx::Dimension Dim> struct kit::yaml::codec<lynx::app<Dim>>
{
    static YAML::Node encode(const lynx::app<Dim> &app)
    {
        YAML::Node node;
        node["Background color"] = app.window()->background_color.rgba;
        for (const auto &layer : app)
            node[layer->id] = *layer;
        return node;
    }
    static bool decode(const YAML::Node &node, lynx::app<Dim> &app)
    {
        if (!node.IsMap())
            return false;

        app.window()->background_color = lynx::color(node["Background color"].as<glm::vec4>());
        for (const auto &layer : app)
            node[layer->id].template as<lynx::layer<Dim>>(*layer);

        return true;
    }
};

#endif