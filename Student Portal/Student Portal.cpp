#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>

#include "database.h"

#ifdef _WIN32
#define CLEAR_CMD "cls"
#else
#define CLEAR_CMD "clear"
#endif

using namespace StudentPortal;

Vector<Entity*> StudentPortal::g_Entities;
Vector<Course*> StudentPortal::g_Courses;
size_t StudentPortal::g_LastEntityId = 0;
size_t StudentPortal::g_LastCourseId = 0;

short stringToShort(const std::string& str) {
	short val = 0;
	for (const auto& c : str) {
		if (!(c >= '0' && c <= '9'))
			return -1;

		val += c - '0';
		val *= 10;
	}

	return val / 10;
}

void studentMenu(Student* student) {
	while (true) {
		printf("Hello %s, your courses:\n", student->getName().c_str());
		for (size_t i = 0; i < student->grades.size(); i++) {
			Course* course = findCourseById(student->grades[i].courseId);
			printf("%lld- %s\n", i + 1, course->getName().c_str());
		}

		if (student->getType() == EntityType::TeachingAssistant) { // checking if the student is an assistant
			auto course = findCourseById(((TeachingAssistant*)student)->getAssistingCourse());
			if (course) 
				printf("\nCourse you are assisting at: %s\n", course->getName().c_str());
		}

		printf("\n1- Register to a new course\n2- Calculate GPA\n3- Log out\n\n");

		std::string selection;
		std::getline(std::cin, selection);

		if (selection == "1") {
			while (true) {
				system(CLEAR_CMD);
				if (student->grades.size() != g_Courses.size()) {
					for (size_t i = 0; i < g_Courses.size(); i++) {
						bool alreadyRegistered = false;
						for (size_t j = 0; j < student->grades.size() && !alreadyRegistered; j++) {
							if (student->grades[j].courseId == g_Courses[i]->getId())
								alreadyRegistered = true;
						}

						if (!alreadyRegistered)
							printf("%d- %s: %s\n", g_Courses[i]->getId(), g_Courses[i]->getName().c_str(), g_Courses[i]->getDesc().c_str());
					}

					printf("\nEnter course id: ");

					std::string input;
					std::getline(std::cin, input);

					auto course = findCourseById(stringToShort(input));

					if (course) {
						course->getStudents().push(student);
						student->registerToCourse(course->getId());

						printf("Successfully registered!\n");
						Sleep(500);
						break;
					}
					else
						printf("Enter a valid id!\n");
				}
				else if (!g_Courses.size()) {
					printf("There are currently no courses to take.\n");
					Sleep(2000);
					break;
				}
				else {
					printf("You have already registered to all courses!\n");
					Sleep(2000);
					break;
				}

				Sleep(2000);
			}
		}
		else if (selection == "2") {
			printf("GPA: %lf", student->calculateGPA());
			Sleep(2000);
		}
		else if (selection == "3")
			return;
		else {
			printf("Enter a valid selection!\n");
			Sleep(500);
		}

		system(CLEAR_CMD);
	}
}

void professorMenu(Professor* prof) {
	while (true) {
		system(CLEAR_CMD);
		printf("Hello Professor %s, courses you are teaching:\n", prof->getName().c_str());

		for (size_t i = 0; i < prof->courses.size(); i++) {
			auto ptr = findCourseById(prof->courses[i]);
			printf("%lld- %s\n", i + 1, ptr->getName().c_str());
		}

		printf("\n\n1 - Grade students\n2 - Assign assistant\n3 - Teach a course\n4- Log out\n\n");
		std::string selection;
		std::getline(std::cin, selection);

		if (selection == "1") {
			system(CLEAR_CMD);

			if (prof->courses.size() != 0) {
				for (size_t i = 0; i < prof->courses.size(); i++) {
					auto ptr = findCourseById(prof->courses[i]);
					if (ptr->getProfessor() == prof)
						printf("%d- %s\n", ptr->getId(), ptr->getName().c_str());
				}

				printf("\nSelect a course: ");

				std::string courseSel;
				std::getline(std::cin, courseSel);

				auto course = findCourseById(stringToShort(courseSel));

				if (course) {
					system(CLEAR_CMD);

					auto& students = course->getStudents();
					for (size_t i = 0; i < students.size(); i++) {
						printf("%d- %s\n", students[i]->getId(), students[i]->getName().c_str());
					}

					printf("\nSelect a student: ");

					std::string stuSel;
					std::getline(std::cin, stuSel);

					auto student = findEntityById<Student>(stringToShort(stuSel));

					if (student) {
						printf("\nEnter a grade (current: %d): ", student->getGrade(course->getId()));

						std::string grade;
						std::getline(std::cin, grade);

						auto gradeVal = stringToShort(grade);
						if (gradeVal != -1) {
							student->setGrade(course->getId(), gradeVal);
							printf("Successfully graded!\n");
						}
						else
							printf("Enter a valid grade!\n");
					}
					else
						printf("Enter a valid student id!\n");
				}
				else {
					printf("Enter a valid course id!\n");
				}
			}
			else
				printf("You are not teaching any courses!\n");

			Sleep(1500);
		}
		else if (selection == "2") {
			system(CLEAR_CMD);

			if (prof->courses.size() != 0) {
				for (size_t i = 0; i < prof->courses.size(); i++) {
					auto ptr = findCourseById(prof->courses[i]);
					if (ptr->getProfessor() == prof)
						printf("%d- %s\n", ptr->getId(), ptr->getName().c_str());
				}

				printf("Select a course: ");

				std::string courseSel;
				std::getline(std::cin, courseSel);

				auto course = findCourseById(stringToShort(courseSel));

				if (course) {
					system(CLEAR_CMD);

					auto& students = course->getStudents();
					for (size_t i = 0; i < students.size(); i++) {
						printf("%d- %s\n", students[i]->getId(), students[i]->getName().c_str());
					}

					printf("\nSelect a student to make assistant: ");

					std::string stuSel;
					std::getline(std::cin, stuSel);

					auto student = findEntityById<Student>(stringToShort(stuSel));
					if (student) {
						for (size_t i = 0; i < g_Entities.size(); i++) {
							if (g_Entities[i]->getId() == student->getId()) {
								auto ta = new TeachingAssistant(*student);
								ta->setAssistingCourse(course->getId());
								g_Entities[i] = ta;
								delete student;
								printf("Successfully assigned assistant!\n");
							}
						}
					}
					else
						printf("Enter a valid student id!\n");
				}
				else {
					printf("Enter a valid course id!\n");
				}
			}
			else
				printf("You are not teaching any courses!\n");


			Sleep(1500);
		}
		else if (selection == "3") {
			system(CLEAR_CMD);

			if (prof->courses.size() != g_Courses.size()) {
				for (size_t i = 0; i < g_Courses.size(); i++) {
					bool alreadyRegistered = false;
					for (size_t j = 0; j < prof->courses.size() && !alreadyRegistered; j++) {
						if (!g_Courses[i]->getProfessor() && prof->courses[j] == g_Courses[i]->getId())
							alreadyRegistered = true;
					}

					if (!alreadyRegistered)
						printf("%d- %s: %s\n", g_Courses[i]->getId(), g_Courses[i]->getName().c_str(), g_Courses[i]->getDesc().c_str());
				}

				printf("\nEnter course id: ");

				std::string input;
				std::getline(std::cin, input);

				auto course = findCourseById(stringToShort(input));

				if (course) {
					course->setProfessor(prof);
					prof->teachCourse(course->getId());
					printf("Successfully registered!\n");
					Sleep(1500);
				}
				else
					printf("Enter a valid id!\n");
			}
			else if (!g_Courses.size()) {
				printf("There are no courses to take atm.\n");
				Sleep(2000);
				break;
			}
			else {
				printf("You have already registered to all courses!\n");
				Sleep(2000);
				break;
			}

			Sleep(1500);
		}
		else if (selection == "4")
			return;
		else {
			printf("Enter a valid selection!\n");
			Sleep(500);
		}

		system(CLEAR_CMD);
	}
}

void adminMenu(Entity* admin) {
	while (true) {
		printf("Admin menu:\n\n1- Create a new student\n2- Create a new professor\n3- Create a new course\n4- Log out\n\n");
		std::string selection;
		std::getline(std::cin, selection);

		if (selection == "1") {
			Entity* e = new Student;

			std::string username, password, fullName;

			printf("Username: ");
			std::getline(std::cin, username);

			printf("Password: ");
			std::getline(std::cin, password);

			printf("Full name: ");
			std::getline(std::cin, fullName);

			e->setId(g_LastEntityId++);
			e->setUsername(username);
			e->setPassword(password);
			e->setName(fullName);

			g_Entities.push(e);

			printf("Successfully created entity!\n");
			Sleep(1500);
		}
		else if (selection == "2") {
			Entity* e = new Professor;

			std::string username, password, fullName;

			printf("Username: ");
			std::getline(std::cin, username);

			printf("Password: ");
			std::getline(std::cin, password);

			printf("Full name: ");
			std::getline(std::cin, fullName);

			e->setId(g_LastEntityId++);
			e->setUsername(username);
			e->setPassword(password);
			e->setName(fullName);

			g_Entities.push(e);

			printf("Successfully created entity!\n");
			Sleep(1500);
		}
		else if (selection == "3") {
			Course* course = new Course;

			std::string name, description;

			printf("Enter course name: ");
			std::getline(std::cin, name);

			printf("Description: ");
			std::getline(std::cin, description);

			course->setId(g_LastCourseId++);
			course->setName(name);
			course->setDesc(description);

			g_Courses.push(course);

			printf("Successfully created entity!\n");
			Sleep(1500);
		}
		else if (selection == "4")
			return;
		else {
			printf("Enter a valid selection!\n");
			Sleep(500);
		}

		system(CLEAR_CMD);
	}
}

int main() {
	const std::string entityDbFile = "entities.bin";
	const std::string courseDbFile = "courses.bin";

	loadEntities(entityDbFile);
	loadCourses(courseDbFile);

	if (!findEntityByUsername<Entity>("admin")) {
		Entity* e = new Entity;

		e->setName("admin");
		e->setUsername("admin");
		e->setPassword("admin");
		e->setId(g_LastEntityId++);
		e->setType(EntityType::SystemAdmin);

		g_Entities.push(e);
	}


	while (true) {
		printf("Welcome to Student Portal\n\n1- Login\n2- Exit\n");

		std::string selection;
		std::getline(std::cin, selection);

		if (selection == "1") {
			system(CLEAR_CMD);

			while (true) {
				std::string username, password;

				printf("Enter your username: ");
				std::getline(std::cin, username);

				printf("Enter your password: ");
				std::getline(std::cin, password);

				auto entity = findEntityByUsername<Entity>(username);

				if (!entity || entity->getPassword().compare(password) != 0) {
					printf("Username or password is wrong!");
					Sleep(500);
				}
				else {
					system(CLEAR_CMD);
					switch (entity->getType()) {
					case EntityType::Student: {
						studentMenu((Student*)entity);
						break;
					}
					case EntityType::Professor: {
						professorMenu((Professor*)entity);
						break;
					}
					case EntityType::TeachingAssistant: {
						studentMenu((Student*)entity);
						break;
					}
					case EntityType::SystemAdmin: {
						adminMenu(entity);
						break;
					}
					}
					break;
				}

				system(CLEAR_CMD);
			}
		}
		else if (selection == "2") {
			break;
		}
		else {
			printf("Invalid selection...\n");
			Sleep(500);
		}

		system(CLEAR_CMD);
	}

	saveCourses(courseDbFile);
	saveEntities(entityDbFile);

	deleteCourses();
	deleteEntities();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
