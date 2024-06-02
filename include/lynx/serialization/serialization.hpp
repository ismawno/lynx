#pragma once
#ifdef KIT_USE_YAML_CPP

#include "kit/serialization/yaml/codec.hpp"
#include "kit/serialization/yaml/glm.hpp"
#include "lynx/app/app.hpp"

template <> struct kit::yaml::codec<lynx::color>
{
    static bool is_float(const std::string &str)
    {
        std::istringstream iss(str);
        float f;
        iss >> std::noskipws >> f;
        return iss.eof() && !iss.fail();
    }
    static YAML::Node encode(const lynx::color &color)
    {
        YAML::Node node;
        node = "#" + color.to_hex_str();
        return node;
    }
    static bool decode(const YAML::Node &node, lynx::color &color)
    {
        if (node.IsSequence())
        {
            if (node.size() != 4 || node.size() != 3)
                return false;
            if (is_float(node[0].Scalar()))
                color = node.size() == 4 ? lynx::color(node.as<glm::vec4>()) : lynx::color(node.as<glm::vec3>());
            else
                color = lynx::color(node[0].as<std::uint32_t>(), node[1].as<std::uint32_t>(),
                                    node[2].as<std::uint32_t>(), node.size() == 4 ? node[3].as<std::uint32_t>() : 255u);
            return true;
        }
        if (!node.IsScalar())
            return false;
        std::string hex = node.as<std::string>();
        if (hex[0] == '#')
            hex = hex.substr(1);
        if (hex.size() != 6 && hex.size() != 8)
            return false;

        color = lynx::color::from_hex(hex, hex.size() == 8);
        return true;
    }
};

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
        node["Background color"] = app.window()->background_color;
        for (const auto &layer : app)
            node[layer->id] = *layer;
        return node;
    }
    static bool decode(const YAML::Node &node, lynx::app<Dim> &app)
    {
        if (!node.IsMap())
            return false;

        app.window()->background_color = node["Background color"].as<lynx::color>();
        for (const auto &layer : app)
            node[layer->id].template as<lynx::layer<Dim>>(*layer);

        return true;
    }
};

#endif