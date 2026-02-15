#pragma once

#include "core/typedefs.h"
#include "core/config/instance.h"


class Resource : public Instance {
    VREGISTER_CLASS(Resource, Instance)

    // Name of the resource
    String name = "";
    // The path on-disk to the resource. 
    String path = "";
public:
    
    /**
     * @brief Getter for the name of the resource.
     * @return The name of the resource.
     */
    String get_name() const;

    /**
     * @brief Setter for the name of the resource.
     * @param p_name The new name of the resource.
     */
    void set_name(const String &p_name);

    /**
     * @brief Getter for the path of the resource.
     * @return The path of the resource.
     */
    String get_path() const;

    /**
     * @brief Setter for the path of the resource.
     * @param p_path The new path to the resource.
     */
    void set_path(const String &p_path);

    Resource() {}
    ~Resource() {}
};
