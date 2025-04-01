#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

const std::string ANSI_REFRESH = "\033[2J\033[H";

// Creates an enum to hold task priority
enum class Priority
{
    low = 1,
    medium = 2,
    high = 3
};


std::ostream& operator<<(std::ostream& os, Priority p)
{
    switch (p)
    {
    case Priority::low:
        os << 1;
        break;
    case Priority::medium:
        os << 2;
        break;
    case Priority::high:
        os << 3;
        break;
    }
    return os;
}

// Defines a struct to hold task information
struct Task
{
    // Instance data
    std::string label;
    bool is_completed;
    Priority task_priority;

    // Contructors
    Task(std::string label) : label(label), is_completed(false), task_priority(Priority::low) {}
    Task(std::string label, bool is_completed, Priority task_priority) : label(label), is_completed(is_completed), task_priority(task_priority) {}

    // Use default destructor
    ~Task() = default;

    // Method to check if task is completed
    bool is_finished() { return this->is_completed; }

    // Method to toggle task status
    void toggle() { this->is_completed = !this->is_completed; }

    // Overloads the << operator for use with output stream
    friend std::ostream& operator<<(std::ostream& os, const Task& task)
    {
        // Handle priority color
        switch (task.task_priority)
        {
        case Priority::low:
            // Green
            os << "\033[32m";
            break;

        case Priority::medium:
            // Yellow
            os << "\033[33m";
            break;

        case Priority::high:
            // Red
            os << "\033[31m";
            break;
        }
        os << '[' << (task.is_completed ? 'X' : ' ') << "] " << task.label;
        
        // Make text white again
        os << "\033[37m";
        return os;
    }
};

std::filesystem::path verify_taskfile()
{
    std::string taskfile_path = std::filesystem::current_path().string() + "/taskfile.csv";

    // Checks if taskfile is already present
    if (std::filesystem::exists(taskfile_path))
    {
        return std::filesystem::path{ taskfile_path };
    }

    // Creates the taskfile
    std::filesystem::path path{ taskfile_path };
    std::ofstream{ path };
    return path;
}

std::vector<Task> read_taskfile(const std::filesystem::path filepath)
{
    // Open the taskfile
    std::ifstream taskfile{ filepath };

    // Create vector to hold tasks
    std::vector<Task> tasks{};

    // Create variable to hold line
    std::string line;

    // Read lines from file
    while (std::getline(taskfile, line))
    {
        std::stringstream ss{ line };
        std::string task_label, task_status_str;
        std::string task_priority;
        std::getline(ss, task_label, ',');
        std::getline(ss, task_status_str, ',');
        std::getline(ss, task_priority);

        // Convert the status from string to bool
        bool task_status = std::stoi(task_status_str) != 0;

        // Cast the priority to enum type
        Priority task_priority_int = static_cast<Priority>(std::stoi(task_priority));

        // Add task to tasklist
        tasks.push_back(Task{ task_label, task_status, task_priority_int });
    }

    return tasks;
}


void list_all_tasks(const std::vector<Task>& task_vec)
{
    // If vector contains no tasks, don't list any
    if (task_vec.empty())
    {
        std::cout << "No tasks... Use command 'create' to create one.\n";
        return;
    }

    // Prints all tasks in vec with index
    int i = 1;
    for (const Task task : task_vec)
    {
        std::cout << i << ": " << task << '\n';
        ++i;
    }
}

void write_taskfile(const std::filesystem::path filepath, const std::vector<Task>& task_vec)
{
    // Open taskfile and write to it
    std::ofstream taskfile{ filepath };
    for (Task task : task_vec)
    {
        taskfile << task.label << ',' << (task.is_finished() ? '1' : '0') << ',' << task.task_priority << '\n';
    }
}

int main()
{
    // Makes sure taskfile is present
    std::filesystem::path taskfile_path = verify_taskfile();

    // Loads tasks from file into vector. TODO
    std::vector<Task> tasks = read_taskfile(taskfile_path);

    // List tasks
    list_all_tasks(tasks);

    // Command loop
    while (true)
    {
        std::string command = "";
        std::cout << "> ";
        std::cin >> command;

        // Handle the command
        if (command == "exit")
        {
            // Write all tasks to taskfile and exit program
            write_taskfile(taskfile_path, tasks);
            return 0;
        }

        else if (command == "save")
        {
            // Write all tasks to taskfile
            write_taskfile(taskfile_path, tasks);
        }

        else if (command == "create")
        {
            std::string label;
            std::getline(std::cin, label);

            // If no name is provided, do not create task
            if (label.empty())
            {
                std::cout << ANSI_REFRESH;
                list_all_tasks(tasks);
            }

            // Remove tralining whitespace
            label.erase(label.begin());

            tasks.push_back(Task(label));
        }

        else if (command == "remove")
        {
            std::string index;
            std::getline(std::cin, index);

            // Converts string to an int
            int index_int = std::stoi(index);

            // If index is out of bounds, do nothing
            if (index_int < 1 || index_int > tasks.size())
            {
                std::cout << ANSI_REFRESH;
                list_all_tasks(tasks);
                continue;
            }

            // Remove task at index
            tasks.erase(tasks.begin() + index_int - 1);
        }

        else if (command == "toggle")
        {
            std::string index;
            std::getline(std::cin, index);

            // Converts string to an int
            int index_int = std::stoi(index);

            // If index is out of bounds, do nothing
            if (index_int < 1 || index_int > tasks.size())
            {
                std::cout << ANSI_REFRESH;
                list_all_tasks(tasks);
                continue;
            }

            // Toggle task at index
            tasks[index_int - 1].toggle();
        }

        else if (command == "priority")
        {
            std::string index;
            std::string priority;

            std::cin >> index >> priority;

            // Converts string to int
            int index_int = std::stoi(index);

            // If index is out of bounds, do nothing
            if (index_int < 1 || index_int > tasks.size())
            {
                std::cout << ANSI_REFRESH;
                list_all_tasks(tasks);
                continue;
            }

            // If priority is not valid, do nothing
            if (priority != "low" && priority != "medium" && priority != "high")
            {
                std::cout << ANSI_REFRESH;
                list_all_tasks(tasks);
                continue;
            }

            // Change priority of task at index
            if (priority == "low")
            {
                tasks[index_int - 1].task_priority = Priority::low;
            }
            else if (priority == "medium")
            {
                tasks[index_int - 1].task_priority = Priority::medium;
            }
            else
            {
                tasks[index_int - 1].task_priority = Priority::high;
            }
        }

        // Refresh task display
        std::cout << "\033[2J\033[H";
        list_all_tasks(tasks);
    }
}