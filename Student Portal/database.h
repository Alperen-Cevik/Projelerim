#pragma once
#include "entity.h"

namespace StudentPortal {
	extern Vector<Entity*> g_Entities;
	extern Vector<Course*> g_Courses;
	extern size_t g_LastEntityId;
	extern size_t g_LastCourseId;

	template <typename T>
	T* findEntityById(short entityId) {
		for (size_t i = 0; i < g_Entities.size(); i++) {
			if (g_Entities[i]->getId() == entityId)
				return (T*)g_Entities[i];
		}

		return NULL;
	}

	template <typename T>
	T* findEntityByUsername(const std::string& username) {
		for (size_t i = 0; i < g_Entities.size(); i++) {
			if (g_Entities[i]->getUsername().compare(username) == 0)
				return (T*)g_Entities[i];
		}

		return NULL;
	}

	Course* findCourseById(short id) {
		for (size_t i = 0; i < g_Courses.size(); i++) {
			if (g_Courses[i]->getId() == id)
				return g_Courses[i];
		}

		return NULL;
	}


	Entity* createEntity(EntityType entityType) {
		Entity* e = NULL;

		switch (entityType) {
		case EntityType::Student:
			e = new Student;
			break;
		case EntityType::Professor:
			e = new Professor;
			break;
		case EntityType::TeachingAssistant:
			e = new TeachingAssistant;
			break;
		case EntityType::SystemAdmin:
			e = new Entity;
			break;
		default:
			return NULL;
		}

		e->setType(entityType);
		return e;
	}

	bool loadEntities(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::binary);

		if (!file.is_open()) {
			return false;
		}

		readPrimitive(file, g_LastEntityId);

		size_t entityCount = 0;
		readPrimitive(file, entityCount);

		g_Entities.clear();
		g_Entities.reserve(entityCount);

		for (size_t i = 0; i < entityCount; i++) {
			EntityType entityType;
			readPrimitive(file, entityType);

			Entity* entity = createEntity(entityType);
			entity->readFromFile(file);

			g_Entities.push(entity);
		}

		return true;
	}

	bool saveEntities(const std::string& fileName) {
		std::ofstream file(fileName, std::ios::binary);

		if (!file.is_open()) {
			printf("Failed to open %s...\n", fileName.c_str());
			return false;
		}

		writePrimitive(file, g_LastEntityId);
		writePrimitive(file, g_Entities.size());

		for (size_t i = 0; i < g_Entities.size(); i++) {
			auto type = g_Entities[i]->getType();
			writePrimitive(file, type);
			g_Entities[i]->writeToFile(file);
		}

		return true;
	}

	bool loadCourses(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::binary);

		if (!file.is_open()) {
			return false;
		}

		readPrimitive(file, g_LastCourseId);

		size_t courseCount = 0;
		readPrimitive(file, courseCount);

		g_Courses.reserve(courseCount);

		for (size_t i = 0; i < courseCount; i++) {
			Course* course = new Course;

			short id;
			readPrimitive(file, id);

			std::string name, desc;
			readString(file, name);
			readString(file, desc);

			course->setId(id);
			course->setName(name);
			course->setDesc(desc);

			short profId = 0;
			readPrimitive(file, profId);
			course->setProfessor(findEntityById<Professor>(profId));

			size_t assistantCount = 0;
			readPrimitive(file, assistantCount);

			auto& assistants = course->getAssistants();
			assistants.reserve(assistantCount);

			for (size_t i = 0; i < assistantCount; i++) {
				short c;
				readPrimitive(file, c);

				assistants.push(findEntityById<TeachingAssistant>(c));
			}

			size_t studentCount = 0;
			readPrimitive(file, studentCount);

			auto& students = course->getStudents();
			students.reserve(studentCount);

			for (size_t i = 0; i < studentCount; i++) {
				short c;
				readPrimitive(file, c);

				students.push(findEntityById<Student>(c));
			}

			g_Courses.push(course);
		}

		return true;
	}

	bool saveCourses(const std::string& fileName) {
		std::ofstream file(fileName, std::ios::binary);

		if (!file.is_open()) {
			printf("Failed to open %s...\n", fileName.c_str());
			return false;
		}

		writePrimitive(file, g_LastEntityId);
		writePrimitive(file, g_Courses.size());

		for (size_t i = 0; i < g_Courses.size(); i++) {
			writePrimitive(file, g_Courses[i]->getId());

			writeString(file, g_Courses[i]->getName());
			writeString(file, g_Courses[i]->getDesc());

			short id = g_Courses[i]->getProfessor() ? g_Courses[i]->getProfessor()->getId() : 0;
			writePrimitive(file, id);

			auto& assistants = g_Courses[i]->getAssistants();
			writePrimitive(file, assistants.size());

			for (size_t i = 0; i < assistants.size(); i++)
				writePrimitive(file, assistants[i]->getId());

			auto& students = g_Courses[i]->getStudents();
			writePrimitive(file, students.size());

			for (size_t i = 0; i < students.size(); i++)
				writePrimitive(file, students[i]->getId());
		}

		return true;
	}

	void deleteEntities() {
		for (size_t i = 0; i < g_Entities.size(); i++)
			delete g_Entities[i];
	}

	void deleteCourses() {
		for (size_t i = 0; i < g_Courses.size(); i++)
			delete g_Courses[i];
	}
}