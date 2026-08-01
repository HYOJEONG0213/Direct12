#pragma once

enum class OBJECT_TYPE : uint8
{
	NONE,
	GAMEOBJECT, // PREFAB
	COMPONENT,
	MATERIAL,
	MESH,
	MESH_DATA,
	SHADER,
	TEXTURE,

	END
};

enum
{
	OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};

class Object
{
public:
	Object(OBJECT_TYPE type);
	virtual ~Object();

	OBJECT_TYPE GetType() { return _objectType; }

	void		   SetName(const wstring &name) { _name = name; }
	const wstring &GetName() { return _name; }

	void		   SetPath(const wstring &path) { _path = path; }
	const wstring &GetPath() { return _path; }

	uint32 GetID() { return _id; }

	virtual void Load(const wstring &path) {}
	virtual void Save(const wstring &path) {}

	// TODO : Instantiate

protected:
	OBJECT_TYPE _objectType = OBJECT_TYPE::NONE;
	wstring		_name;
	wstring		_path;

protected:
	uint32 _id = 0;
};
