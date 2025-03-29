#include <iostream>
#include <filesystem>
#include <string>
#include <unordered_map>

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

    // Overloads the << operator for use with output stream
    friend std::ostream& operator<<(std::ostream& os, const Task& task)
    {
        os << "[" << (task.is_completed ? 'X' : ' ') << "] " << task.label;
        return os;
    }
};

void verify_taskfile()
{
    // Checks if taskfile is already present
    if (std::filesystem::exists("tasks.csv"))
    {
        std::cout << "Taskfile found";
        return;
    }

    // Creates the taskfile
    std::string taskfile_path = std::filesystem::current_path().string() + "/taskfile.csv";
    std::filesystem::path path{ taskfile_path };
    
    std::cout << "Created taskfile...\n";
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
        std::cout << i << ": " << task << "\n";
        ++i;
    }
}

int main()
{
    // Makes sure taskfile is present
    //verify_taskfile();

    // Creates a vector to store all tasks
    std::vector<Task> tasks{};

    // Loads tasks from file into vector. TODO

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
            // Saves current tasks to taskfile. TODO
        }

        else if (command == "create")
        {
            std::string label;
            std::getline(std::cin, label);

            // If no name is provided, do not create task
            tasks.push_back(Task(label));
        }

        else if (command == "remove")
        {
            std::string index;
            std::getline(std::cin, index);

            // Converts string from output to an int
            int index_int = atoi(index.c_str());

            // If index is out of bounds, do nothing
            if (index_int < 1 || index_int > tasks.size())
            {
                continue;
            }

            // Remove task at index
            tasks.erase(tasks.begin() + index_int - 1);
        }

        // Refresh task display
        std::cout << "\033[2J\033[H";
        list_all_tasks(tasks);
    }
}