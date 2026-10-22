#pragma once
#include <vector>
#include <memory>
#include <utility>
class Object;
enum class EntityType;

class ObjectHandler
{
public:
	ObjectHandler();
	~ObjectHandler();

	template<class T, class... Args>
	T& Spawn(Args&&... args) {
		auto obj = std::make_unique<T>(std::forward<Args>(args)...);
		T& ref = *obj;
		obj->SetId(next_id++);
		pending.push_back(std::move(obj));
		return ref;
	}

	void EndTurn();

	void RemoveType(EntityType type);

	std::vector<std::unique_ptr<Object>>& Objects() { return objects; }
private:
	std::vector<std::unique_ptr<Object>> objects, pending;
	int next_id = 0;
};