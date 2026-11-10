#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <iomanip>
#include <windows.h>

using namespace std;

// Структура процесса по методичке + дополнительные поля для удобства
struct Process {
    string pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;       // 1 = наивысший приоритет 
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    bool is_completed;
    int current_queue;  // Для MLFQ
    double vruntime;    // Для CFS
};

// Структура для диаграммы Ганта
struct GanttEntry {
    string pid;
    int start;
    int end;
};

// Глобальные переменные для ввода
int n_processes;
int time_quantum;

// Ввод данных с консоли
void inputData(vector<Process>& processes) {
    cout << "Введите количество процессов: ";
    cin >> n_processes;

    // Защита от некорректного ввода
    if (n_processes <= 0) {
        cout << "[Предупреждение] Количество процессов должно быть > 0. Установлено значение 1.\n";
        n_processes = 1;
    }

    processes.clear(); // Очищаем вектор на случай повторного вызова

    for (int i = 0; i < n_processes; i++) {
        Process p;
        cout << "\nПроцесс " << i + 1 << ":\n";
        cout << "  PID: ";
        cin >> p.pid;
        cout << "  Время прибытия: ";
        cin >> p.arrival_time;
        cout << "  Время исполнения: ";
        cin >> p.burst_time;
        cout << "  Приоритет: ";
        cin >> p.priority;
        
        p.remaining_time = p.burst_time;
        p.is_completed = false;
        p.start_time = -1;
        p.completion_time = 0;
        p.waiting_time = 0;
        p.turnaround_time = 0;
        p.current_queue = 0; 
        p.vruntime = 0.0;    
        
        processes.push_back(p);
    }

    cout << "\nВведите квант времени для алгоритмов Round Robin и MLFQ: ";
    cin >> time_quantum;
}

// Вспомогательная функция: сортировка по времени прибытия
void sortByArrival(vector<Process>& p) {
    sort(p.begin(), p.end(), [](const Process& a, const Process& b) {
        return a.arrival_time < b.arrival_time;
    });
}

// Вспомогательная функция: проверка, все ли процессы завершены
bool allCompleted(const vector<Process>& p) {
    for (const auto& proc : p) {
        if (!proc.is_completed) return false;
    }
    return true;
}

// Вспомогательная функция: вывод диаграммы Ганта
void printGanttChart(const vector<GanttEntry>& gantt) {
    cout << "\nДиаграмма Ганта:\n";
    for (const auto& entry : gantt) {
        cout << "| " << entry.pid << "(" << entry.start << "-" << entry.end << ") ";
    }
    cout << "|\n";
}

// Вспомогательная функция: расчет и вывод метрик
void printMetrics(vector<Process>& p) {
    double total_wt = 0, total_tat = 0;
    
    cout << "\nИтоговая таблица:\n";
    cout << left << setw(5) << "PID" 
         << setw(5) << "AT" << setw(5) << "BT" 
         << setw(5) << "CT" << setw(6) << "TAT" << setw(5) << "WT" << "\n";
    cout << string(35, '-') << "\n";
    
    for (auto& proc : p) {
        proc.turnaround_time = proc.completion_time - proc.arrival_time;
        proc.waiting_time = proc.turnaround_time - proc.burst_time;
        
        total_wt += proc.waiting_time;
        total_tat += proc.turnaround_time;
        
        cout << left << setw(5) << proc.pid 
             << setw(5) << proc.arrival_time 
             << setw(5) << proc.burst_time 
             << setw(5) << proc.completion_time 
             << setw(6) << proc.turnaround_time 
             << setw(5) << proc.waiting_time << "\n";
    }
    
    cout << "\nСреднее время ожидания: " << fixed << setprecision(2) << total_wt / n_processes << "\n";
    cout << "Среднее время оборота: " << fixed << setprecision(2) << total_tat / n_processes << "\n";
}

// АЛГОРИТМЫ ПЛАНИРОВАНИЯ

// 1. FCFS Невытесняющий
void fcfs(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    vector<GanttEntry> gantt;

    for (auto& proc : p) {
        if (current_time < proc.arrival_time) {
            current_time = proc.arrival_time;
        }
        proc.start_time = current_time;
        gantt.push_back({proc.pid, current_time, current_time + proc.burst_time});
        current_time += proc.burst_time;
        proc.completion_time = current_time;
        proc.is_completed = true;
    }
    
    cout << "\n--- FCFS ---\n";
    printGanttChart(gantt);
    printMetrics(p);
}

// 2. SJF Невытесняющий
void sjf(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;

    while (completed < n_processes) {
        int idx = -1;
        int min_bt = 1e9;
        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && p[i].arrival_time <= current_time) {
                if (p[i].burst_time < min_bt) {
                    min_bt = p[i].burst_time;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            p[idx].start_time = current_time;
            gantt.push_back({p[idx].pid, current_time, current_time + p[idx].burst_time});
            current_time += p[idx].burst_time;
            p[idx].completion_time = current_time;
            p[idx].is_completed = true;
            completed++;
        } else {
            current_time++; // Ждем прибытия следующего процесса
        }
    }
    cout << "\n--- SJF ---\n";
    printGanttChart(gantt);
    printMetrics(p);
}

// 3. Приоритетное Невытесняющий
void priority_np(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;

    while (completed < n_processes) {
        int idx = -1;
        int min_priority = 1e9; // Меньше число = выше приоритет
        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && p[i].arrival_time <= current_time) {
                if (p[i].priority < min_priority) {
                    min_priority = p[i].priority;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            p[idx].start_time = current_time;
            gantt.push_back({p[idx].pid, current_time, current_time + p[idx].burst_time});
            current_time += p[idx].burst_time;
            p[idx].completion_time = current_time;
            p[idx].is_completed = true;
            completed++;
        } else {
            current_time++;
        }
    }
    cout << "\n--- Приоритетное планирование ---\n";
    printGanttChart(gantt);
    printMetrics(p);
}

// 4. HRRN Невытесняющий
void hrrn(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;

    while (completed < n_processes) {
        int idx = -1;
        double max_rr = -1.0;
        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && p[i].arrival_time <= current_time) {
                int waiting_time = current_time - p[i].arrival_time;
                double rr = (double)(waiting_time + p[i].burst_time) / p[i].burst_time;
                if (rr > max_rr) {
                    max_rr = rr;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            p[idx].start_time = current_time;
            gantt.push_back({p[idx].pid, current_time, current_time + p[idx].burst_time});
            current_time += p[idx].burst_time;
            p[idx].completion_time = current_time;
            p[idx].is_completed = true;
            completed++;
        } else {
            current_time++;
        }
    }
    cout << "\n--- HRRN ---\n";
    printGanttChart(gantt);
    printMetrics(p);
}

// 5. LRT Невытесняющий
void lrt(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;

    while (completed < n_processes) {
        int idx = -1;
        int max_bt = -1;
        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && p[i].arrival_time <= current_time) {
                if (p[i].burst_time > max_bt) {
                    max_bt = p[i].burst_time;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            p[idx].start_time = current_time;
            gantt.push_back({p[idx].pid, current_time, current_time + p[idx].burst_time});
            current_time += p[idx].burst_time;
            p[idx].completion_time = current_time;
            p[idx].is_completed = true;
            completed++;
        } else {
            current_time++;
        }
    }
    cout << "\n--- LRT ---\n";
    printGanttChart(gantt);
    printMetrics(p);
}

// 6. Round Robin Вытесняющий
void round_robin(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;
    queue<int> q;
    vector<bool> in_queue(n_processes, false);

    q.push(0);
    in_queue[0] = true;
    current_time = p[0].arrival_time;

    while (completed < n_processes) {
        if (q.empty()) {
            for (int i = 0; i < n_processes; i++) {
                if (!p[i].is_completed && !in_queue[i]) {
                    q.push(i);
                    in_queue[i] = true;
                    current_time = max(current_time, p[i].arrival_time);
                    break;
                }
            }
            if (q.empty()) { current_time++; continue; }
        }

        int idx = q.front();
        q.pop();
        in_queue[idx] = false;

        int exec_time = min(p[idx].remaining_time, time_quantum);
        if (p[idx].start_time == -1) p[idx].start_time = current_time;
        
        gantt.push_back({p[idx].pid, current_time, current_time + exec_time});
        current_time += exec_time;
        p[idx].remaining_time -= exec_time;

        if (p[idx].remaining_time == 0) {
            p[idx].completion_time = current_time;
            p[idx].is_completed = true;
            completed++;
        }

        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && !in_queue[i] && p[i].arrival_time <= current_time) {
                q.push(i);
                in_queue[i] = true;
            }
        }

        if (p[idx].remaining_time > 0) {
            q.push(idx);
            in_queue[idx] = true;
        }
    }
    
    // Оптимизация вывода Ганта
    vector<GanttEntry> optimized_gantt;
    for (const auto& entry : gantt) {
        if (!optimized_gantt.empty() && optimized_gantt.back().pid == entry.pid && optimized_gantt.back().end == entry.start) {
            optimized_gantt.back().end = entry.end;
        } else {
            optimized_gantt.push_back(entry);
        }
    }

    cout << "\n--- Round Robin ---\n";
    printGanttChart(optimized_gantt);
    printMetrics(p);
}

// 7. SRTF Вытесняющий
void srtf(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;

    while (completed < n_processes) {
        int idx = -1;
        int min_rt = 1e9;
        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && p[i].arrival_time <= current_time) {
                if (p[i].remaining_time < min_rt) {
                    min_rt = p[i].remaining_time;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            if (p[idx].start_time == -1) p[idx].start_time = current_time;
            
            // Выполняем 1 единицу времени 
            gantt.push_back({p[idx].pid, current_time, current_time + 1});
            current_time++;
            p[idx].remaining_time--;

            if (p[idx].remaining_time == 0) {
                p[idx].completion_time = current_time;
                p[idx].is_completed = true;
                completed++;
            }
        } else {
            current_time++;
        }
    }
    
    // Оптимизация вывода Ганта объединение соседних одинаковых PID
    vector<GanttEntry> optimized_gantt;
    for (const auto& entry : gantt) {
        if (!optimized_gantt.empty() && optimized_gantt.back().pid == entry.pid && optimized_gantt.back().end == entry.start) {
            optimized_gantt.back().end = entry.end;
        } else {
            optimized_gantt.push_back(entry);
        }
    }

    cout << "\n--- SRTF ---\n";
    printGanttChart(optimized_gantt);
    printMetrics(p);
}

// 8. Приоритетное с вытеснением
void priority_p(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;

    while (completed < n_processes) {
        int idx = -1;
        int min_priority = 1e9;
        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && p[i].arrival_time <= current_time) {
                if (p[i].priority < min_priority) {
                    min_priority = p[i].priority;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            if (p[idx].start_time == -1) p[idx].start_time = current_time;
            
            gantt.push_back({p[idx].pid, current_time, current_time + 1});
            current_time++;
            p[idx].remaining_time--;

            if (p[idx].remaining_time == 0) {
                p[idx].completion_time = current_time;
                p[idx].is_completed = true;
                completed++;
            }
        } else {
            current_time++;
        }
    }

    vector<GanttEntry> optimized_gantt;
    for (const auto& entry : gantt) {
        if (!optimized_gantt.empty() && optimized_gantt.back().pid == entry.pid && optimized_gantt.back().end == entry.start) {
            optimized_gantt.back().end = entry.end;
        } else {
            optimized_gantt.push_back(entry);
        }
    }

    cout << "\n--- Приоритетное планирование ---\n";
    printGanttChart(optimized_gantt);
    printMetrics(p);
}

// 9. MLFQ многоуровневая обратная связь
void mlfq(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;
    
    queue<int> q[3];
    vector<bool> in_queue(n_processes, false);

    q[0].push(0);
    in_queue[0] = true;
    current_time = p[0].arrival_time;

    while (completed < n_processes) {
        int exec_level = -1;
        for (int i = 0; i < 3; i++) {
            if (!q[i].empty()) {
                exec_level = i;
                break;
            }
        }

        if (exec_level == -1) {
            for (int i = 0; i < n_processes; i++) {
                if (!p[i].is_completed && !in_queue[i]) {
                    q[0].push(i);
                    in_queue[i] = true;
                    current_time = max(current_time, p[i].arrival_time);
                    break;
                }
            }
            continue;
        }

        int idx = q[exec_level].front();
        q[exec_level].pop();
        in_queue[idx] = false;

        int quantum = (exec_level == 0) ? time_quantum : ((exec_level == 1) ? time_quantum * 2 : p[idx].remaining_time);
        int exec_time = min(p[idx].remaining_time, quantum);

        if (p[idx].start_time == -1) p[idx].start_time = current_time;
        
        gantt.push_back({p[idx].pid, current_time, current_time + exec_time});
        current_time += exec_time;
        p[idx].remaining_time -= exec_time;

        if (p[idx].remaining_time == 0) {
            p[idx].completion_time = current_time;
            p[idx].is_completed = true;
            completed++;
        }

        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && !in_queue[i] && p[i].arrival_time <= current_time) {
                q[0].push(i);
                in_queue[i] = true;
            }
        }

        if (p[idx].remaining_time > 0) {
            int next_level = min(exec_level + 1, 2);
            q[next_level].push(idx);
            in_queue[idx] = true;
        }
    }

    vector<GanttEntry> optimized_gantt;
    for (const auto& entry : gantt) {
        if (!optimized_gantt.empty() && optimized_gantt.back().pid == entry.pid && optimized_gantt.back().end == entry.start) {
            optimized_gantt.back().end = entry.end;
        } else {
            optimized_gantt.push_back(entry);
        }
    }

    cout << "\n--- MLFQ ---\n";
    printGanttChart(optimized_gantt);
    printMetrics(p);
}

// 10. CFS Completely Fair Scheduler
void cfs(vector<Process> p) {
    sortByArrival(p);
    int current_time = 0;
    int completed = 0;
    vector<GanttEntry> gantt;

    while (completed < n_processes) {
        int idx = -1;
        double min_vruntime = 1e18;
        
        for (int i = 0; i < n_processes; i++) {
            if (!p[i].is_completed && p[i].arrival_time <= current_time) {
                // Вес обратно пропорционален приоритету: приоритет 1 = вес 10, приоритет 10 = вес 1
                double weight = 11.0 - p[i].priority; 
                if (p[i].vruntime < min_vruntime) {
                    min_vruntime = p[i].vruntime;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            if (p[idx].start_time == -1) p[idx].start_time = current_time;
            
            // Выполняем 1 единицу времени для точности
            gantt.push_back({p[idx].pid, current_time, current_time + 1});
            current_time++;
            p[idx].remaining_time--;
            
            // Обновляем виртуальное время
            double weight = 11.0 - p[idx].priority;
            p[idx].vruntime += (1.0 * 1024.0) / weight;

            if (p[idx].remaining_time == 0) {
                p[idx].completion_time = current_time;
                p[idx].is_completed = true;
                completed++;
            }
        } else {
            current_time++;
        }
    }

    vector<GanttEntry> optimized_gantt;
    for (const auto& entry : gantt) {
        if (!optimized_gantt.empty() && optimized_gantt.back().pid == entry.pid && optimized_gantt.back().end == entry.start) {
            optimized_gantt.back().end = entry.end;
        } else {
            optimized_gantt.push_back(entry);
        }
    }

    cout << "\n--- CFS ---\n";
    printGanttChart(optimized_gantt);
    printMetrics(p);
}


// ГЛАВНАЯ ФУНКЦИЯ
int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    
    vector<Process> processes;
    inputData(processes);

    int choice;
    do {
        cout << "\n========================================\n";
        cout << "   МОДЕЛИРОВАНИЕ ПЛАНИРОВЩИКА CPU\n";
        cout << "========================================\n";
        cout << "  НЕВЫТЕСНЯЮЩИЕ:\n";
        cout << "  1. FCFS\n";
        cout << "  2. SJF\n";
        cout << "  3. Приоритетное\n";
        cout << "  4. HRRN\n";
        cout << "  5. LRT\n";
        cout << "  ВЫТЕСНЯЮЩИЕ:\n";
        cout << "  6. Round Robin\n";
        cout << "  7. SRTF\n";
        cout << "  8. Приоритетное\n";
        cout << "  9. MLFQ\n";
        cout << "  10. CFS\n";
        cout << "  11. Запустить ВСЕ алгоритмы\n";
        cout << "  0. Выход\n";
        cout << "Выберите операцию: ";
        cin >> choice;

        switch (choice) {
            case 1: fcfs(processes); break;
            case 2: sjf(processes); break;
            case 3: priority_np(processes); break;
            case 4: hrrn(processes); break;
            case 5: lrt(processes); break;
            case 6: round_robin(processes); break;
            case 7: srtf(processes); break;
            case 8: priority_p(processes); break;
            case 9: mlfq(processes); break;
            case 10: cfs(processes); break;
            case 11: 
                fcfs(processes); sjf(processes); priority_np(processes); 
                hrrn(processes); lrt(processes); round_robin(processes); 
                srtf(processes); priority_p(processes); mlfq(processes); cfs(processes); 
                break;
            case 0: cout << "Завершение работы программы.\n"; break;
            default: cout << "Некорректный выбор. Попробуйте снова.\n";
        }
    } while (choice != 0);
    
    cout << "\nНажмите Enter для выхода...";
    cin.ignore();
    cin.get();
    return 0;
}