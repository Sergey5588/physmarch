#include"scene.h"

void Scene::update_ln() {
	for(int i = 0; i < T__LENGTH; ++i) {
		lengths[i] = 0;
	}

	for(auto i:objects) {
		lengths[i.type]++;
	}
}

void Scene::add_object(Object obj, std::string name) {
	for(int i = 0; i < objects.size(); ++i) {
		if(objects[i].type == obj.type) {
			objects.insert(objects.begin()+i, obj);
            //lengths[obj.type]++;
			labels.insert(labels.begin()+i,name);
			break;
		}
	}
}