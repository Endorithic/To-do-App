#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

const std::string ANSI_REFRESH = "\033[2J\033[H";

// Defines a struct to hold task information
struct Task
{
    // Instance data
    std::string label;
    bool is_completed;

    // Contructors
    Task() : label(), is_completed() {}
    Task(std::string label) : label(label), is_completed(false) {}
    Task(std::string label, bool is_completed) : label(label), is_completed(is_completed) {}

    // Use default destructor
    ~Task() = default;

    // Method to check if task is completed
    bool is_finished() { return this->is_completed; }

    // Method to toggle task status
    void toggle() { this->is_completed = !this->is_completed; }

    // Overloads the << operator for use with output stream
    friend std::ostream& operator<<(std::ostream& os, const Task& task)
    {
        os << '[' << (task.is_completed ? 'X' : ' ') << "] " << task.label;
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
        std::getline(ss, task_label, ',');
        std::getline(ss, task_status_str);

        // Convert the status from string to bool
        bool task_status = atoi(task_status_str.c_str()) == 0 ? false : true;

        // Add task to tasklist
        tasks.push_back(Task{ task_label, task_status });
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
        taskfile << task.label << ',' << (task.is_finished() ? '1' : '0') << '\n';
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

            tasks.push_back(Task(label));
        }

        else if (command == "remove")
        {
            std::string index;
            std::getline(std::cin, index);

            // Converts string to an int
            int index_int = atoi(index.c_str());

            // If index is out of bounds, do nothing
            if (index_int < 1 || index_int > tasks.size())
            {
                std::cout << "\033[2J\033[H";
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
            int index_int = atoi(index.c_str());

            // If index is out of bounds, do nothing
            if (index_int < 1 || index_int > tasks.size())
            {
                std::cout << "\033[2J\033[H";
                list_all_tasks(tasks);
                continue;
            }

            // Toggle task at index
            tasks[index_int - 1].toggle();
        }

        // Refresh task display
        std::cout << "\033[2J\033[H";
        list_all_tasks(tasks);
    }
}