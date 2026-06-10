#pragma once

#include "scene/gui/container.h"

class VRUNTIME_API VBoxContainer : public Container {
	VREGISTER_CLASS(VBoxContainer, Container);

	// Separation distance between any two given objects.
	int separation;

	void _resize();

protected:
	void _notification(int p_what);

public:
	/**
	 * @brief Gets the separation distance between any two sub-objects. Separation distance does not apply to the space
	 * prior to the first object and after the last object.
	 * @return The separation distance between two sub-objects.
	 */
	int get_separation() const;

	/**
	 * @brief Sets the separation distance between any two sub-objects. Separation distance does not apply to the space
	 * prior to the first object and after the last object.
	 * @param p_separation The separation distance to apply.
	 */
	void set_separation(int p_separation);

	VBoxContainer();
};
