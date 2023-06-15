#pragma once
#include <fstream>
#include <string>

#include "vector.h"

inline void writeString(std::ofstream& fileStream, const std::string& data) {
    size_t size = data.size();
    fileStream.write((char*)&size, sizeof(size_t));
    fileStream.write(data.data(), data.size());
}

inline void readString(std::ifstream& fileStream, std::string& out) {
    size_t size = 0;
    fileStream.read((char*)&size, sizeof(size_t));
    out.resize(size);
    fileStream.read(&out[0], size);
}

template<typename T>
inline void writePrimitive(std::ofstream& fileStream, T data) {
    fileStream.write((char*)&data, sizeof(T));
}

template<typename T>
inline void readPrimitive(std::ifstream& fileStream, T& data) {
    fileStream.read((char*)&data, sizeof(T));
}

namespace StudentPortal {
    enum class EntityType {
        None = 0,
        Student,
        Professor,
        TeachingAssistant,
        SystemAdmin
    };

    class Entity {
    public:
        Entity() : entityType(EntityType::None), entityId(0), username(""), password(""), name("") {

        };

        virtual ~Entity() = default;

        virtual void writeToFile(std::ofstream& fStream) {
            writePrimitive(fStream, entityId);
            writeString(fStream, username);
            writeString(fStream, password);
            writeString(fStream, name);
        }

        virtual void readFromFile(std::ifstream& fStream) { 
            readPrimitive(fStream, entityId);
            readString(fStream, username);
            readString(fStream, password);
            readString(fStream, name);
        }

        void setId(short id) {
            this->entityId = id;
        }
        short getId() {
            return entityId;
        }

        EntityType getType() {
            return entityType;
        }
        void setType(EntityType entityType) {
            this->entityType = entityType;
        }

        std::string getUsername() {
            return username;
        }
        void setUsername(const std::string& username) {
            this->username = username;
        }

        std::string getPassword() {
            return password;
        }
        void setPassword(const std::string& password) {
            this->password = password;
        }

        std::string getName() {
            return name;
        }
        void setName(const std::string& name) {
            this->name = name;
        }

    protected:
        EntityType entityType;
        short entityId;

        std::string username, password, name;
    };

    class Course; // forward declaration

    struct Grade {
        short courseId;
        short grade;
    };

    class Student : public Entity {
    public:
        Student() {
            entityType = EntityType::Student;
        }

        virtual ~Student() = default;

        virtual void writeToFile(std::ofstream& fStream) {
            Entity::writeToFile(fStream);

            writePrimitive(fStream, grades.size());
            
            for (size_t i = 0; i < grades.size(); i++) {
                writePrimitive(fStream, grades[i].courseId);
                writePrimitive(fStream, grades[i].grade);
            }
        }

        virtual void readFromFile(std::ifstream& fStream) {
            Entity::readFromFile(fStream);

            size_t gradeCount = 0;
            readPrimitive(fStream, gradeCount);

            grades.reserve(gradeCount);

            for (size_t i = 0; i < gradeCount; i++) {
                Grade grade;
                
                readPrimitive(fStream, grade.courseId);
                readPrimitive(fStream, grade.grade);

                grades.push(grade);
            }
        }

        short getGrade(short id) {
            for (size_t i = 0; i < grades.size(); i++) {
                if (id == grades[i].courseId)
                    return grades[i].grade;
            }

            return 0;
        }
        
        void setGrade(short id, short val) {
            for (size_t i = 0; i < grades.size(); i++) {
                if (id == grades[i].courseId) {
                    grades[i].grade = val;
                    return; 
                }
            }
        }

        double calculateGPA() {
            double sum = 0.0;

            if (!grades.size())
                return 0.0;

            for (size_t i = 0; i < grades.size(); i++)
                sum += grades[i].grade;

            return sum / grades.size();
        }

        void registerToCourse(short courseId) {
            Grade grade;
            grade.courseId = courseId;
            grade.grade = 0;

            grades.push(grade);
        }

        Vector<Grade> grades;
    };

    class TeachingAssistant final : public Student {
    public:
        TeachingAssistant() : course(NULL) {

        }

        TeachingAssistant(Student& student) : course(NULL) {
            setId(student.getId());
            setUsername(student.getUsername());
            setPassword(student.getPassword());
            setName(student.getName());
            setType(EntityType::TeachingAssistant);
        }

        void writeToFile(std::ofstream& fStream) {
            Student::writeToFile(fStream);

            writePrimitive(fStream, course);
        }

        void readFromFile(std::ifstream& fStream) {
            Student::readFromFile(fStream);

            readPrimitive(fStream, course);
        }

        short getAssistingCourse() {
            return course;
        }
        void setAssistingCourse(short course) {
            this->course = course;
        }

    private:
        short course;
    };


    /*
        grade assignments,
        teach course,
        assign assistants
    */
    class Professor final : public Entity {
    public:
        Professor() {
            entityType = EntityType::Professor;
        }

        void writeToFile(std::ofstream& fStream) {
            Entity::writeToFile(fStream);

            writePrimitive(fStream, courses.size());

            for (size_t i = 0; i < courses.size(); i++)
                writePrimitive(fStream, courses[i]);
        
        }

        void readFromFile(std::ifstream& fStream) {
            Entity::readFromFile(fStream);

            size_t size = 0;
            readPrimitive(fStream, size);

            courses.reserve(size);

            for (size_t i = 0; i < size; i++) {
                short c;
                readPrimitive(fStream, c);
                
                courses.push(c);
            }
        }

        void teachCourse(short courseId) {
            courses.push(courseId);
        }
 
        Vector<short> courses;
    };

    class Course {
    public:
        Course() : id(0), name(""), description(""), professor(NULL) {
           
        }

        short getId() {
            return id;
        }
        void setId(short id) {
            this->id = id;
        }

        std::string getName() {
            return name;
        }
        void setName(const std::string& name) {
            this->name = name;
        }

        std::string getDesc() {
            return description;
        }
        void setDesc(const std::string& desc) {
            description = desc;
        }

        Professor* getProfessor() {
            return professor;
        }
        void setProfessor(Professor* prof) {
            this->professor = prof;
        }

        Vector<Student*>& getStudents() {
            return students;
        }

        Vector<TeachingAssistant*>& getAssistants() {
            return assistants;
        }

    private:
        short id;
        std::string name, description;

        Professor* professor;
        Vector<Student*> students;
        Vector<TeachingAssistant*> assistants;
    };
}
