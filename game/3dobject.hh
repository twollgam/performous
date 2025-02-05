#pragma once

#include "fs.hh"
#include "graphic/glutil.hh"
#include <memory>
#include <string>

// TODO: Exception handling
// TODO: Texture loading

class Texture;
class Window;

/// A class representing 3d object
/// Non-copyable because of display lists getting messed up
class Object3d {
  public:
	Object3d(const Object3d&) = delete;
	const Object3d& operator=(const Object3d&) = delete;
	Object3d() {}
	Object3d(fs::path const& filepath, fs::path const& texturepath = fs::path(), float scale = 1.0f) {
		load(filepath, texturepath, scale);
	}
	/// load a new object file
	void load(fs::path const& filepath, fs::path const& texturepath = fs::path(), float scale = 1.0f);
	/// draws the object (with texture if given)
	void draw(Window&);
	/// draws the object with a transform
	void draw(Window&, float x, float y, float z = 0.0f, float s = 1.0f);

  private:
	/// load a Wavefront .obj 3d object file
	void loadWavefrontObj(fs::path const& filepath, float scale = 1.0f);
	glutil::VertexArray m_va;
	std::unique_ptr<Texture> m_texture; /// texture
};
