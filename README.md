# cpu-scheduler-simulator

Учебный репозиторий. Консольное приложение на C++, моделирующее работу планировщика CPU операционной системы и демонстрирующее классические вытесняющие и невытесняющие алгоритмы планирования. Пользователь вводит параметры процессов через консоль, а программа рассчитывает метрики эффективности и визуализирует выполнение в виде диаграммы Ганта.

**Реализованные алгоритмы:**

*   **FCFS (First-Come, First-Served).** Невытесняющий алгоритм. Процессы выполняются строго в порядке их поступления в очередь готовности. Главный недостаток — риск «эффекта конвоя», когда один длинный процесс блокирует выполнение множества коротких задач.
*   **SJF (Shortest Job First).** Невытесняющий алгоритм. Планировщик выбирает процесс с наименьшим ожидаемым временем исполнения. Обеспечивает минимальное среднее время ожидания, но подвержен риску «голодания» длинных задач.
*   **Приоритетное планирование (Priority Scheduling).** Каждому процессу присваивается числовой приоритет. Может работать как в вытесняющем, так и в невытесняющем режиме. Риск голодания низкоприоритетных процессов решается механизмом динамического повышения приоритета.
*   **HRRN (Highest Response Ratio Next).** Невытесняющий алгоритм, компенсирующий недостатки SJF. Вычисляет динамический коэффициент отклика: Чем дольше процесс ждет, тем выше его шанс на получение CPU.
*   **LRT (Longest Run Time).** Невытесняющий алгоритм, обратный SJF. Преимущество отдается самым длительным задачам. Теоретическая дисциплина, редко применяемая на практике из-за плохого времени отклика для интерактивных систем.
*   **Round Robin (RR).** Классический вытесняющий алгоритм для систем с разделением времени. Каждому процессу выделяется фиксированный квант времени. По истечении кванта таймер генерирует прерывание, и процесс помещается в конец очереди готовности.
*   **SRTF (Shortest Remaining Time First).** Вытесняющая версия SJF. Если в систему поступает новый процесс, чье полное время выполнения меньше оставшегося времени текущего процесса, происходит немедленное вытеснение. Требует высоких накладных расходов на переключение контекста.
*   **MLFQ (Multilevel Feedback Queue).** Вытесняющий адаптивный алгоритм, используемый в современных десктопных ОС. Процессы перемещаются между несколькими очередями с разными приоритетами и размерами квантов: исчерпавшие квант понижаются в приоритете, а быстро блокирующиеся на вводе-выводе — повышаются.
*   **CFS (Completely Fair Scheduler).** Вытесняющий алгоритм, используемый по умолчанию в ядре Linux. Вместо жестких квантов вычисляет виртуальное время выполнения с учетом весовых коэффициентов приоритетов, стремясь предоставить каждому процессу идеально справедливую долю процессорного времени.

**Статус:** Архив / Read-Only. Проект завершен, разработка и внесение изменений не планируются.

---

# cpu-scheduler-simulator

Educational repository. A C++ console application simulating the operation of an operating system's CPU scheduler and demonstrating classic preemptive and non-preemptive scheduling algorithms. The user inputs process parameters via the console, and the program calculates efficiency metrics and visualizes execution as a Gantt chart.

Implemented algorithms:

*   **FCFS (First-Come, First-Served).** A non-preemptive algorithm. Processes are executed strictly in the order they arrive in the ready queue. The main drawback is the risk of the "convoy effect," where one long process blocks the execution of many short tasks.
*   **SJF (Shortest Job First).** A non-preemptive algorithm. The scheduler selects the process with the shortest expected execution time. It ensures the minimum average waiting time but is susceptible to the risk of "starvation" of long tasks.
*   **Priority Scheduling.** Each process is assigned a numerical priority. It can operate in both preemptive and non-preemptive modes. The risk of starvation for low-priority processes is mitigated by a dynamic priority increase mechanism.
*   **HRRN (Highest Response Ratio Next).** A non-preemptive algorithm that compensates for the drawbacks of SJF. It calculates a dynamic response ratio: the longer a process waits, the higher its chance of getting the CPU.
*   **LRT (Longest Run Time).** A non-preemptive algorithm, the reverse of SJF. Preference is given to the longest tasks. A theoretical discipline rarely used in practice due to poor response times for interactive systems.
*   **Round Robin (RR).** A classic preemptive algorithm for time-sharing systems. Each process is allocated a fixed time quantum. Upon expiration of the quantum, the timer triggers an interrupt, and the process is placed at the end of the ready queue.
*   **SRTF (Shortest Remaining Time First).** The preemptive version of SJF. If a new process arrives in the system whose total execution time is less than the remaining time of the current process, immediate preemption occurs. It requires high context-switching overhead.
*   **MLFQ (Multilevel Feedback Queue).** A preemptive adaptive algorithm used in modern desktop OSs. Processes move between multiple queues with different priorities and quantum sizes: those exhausting their quantum are demoted in priority, while those blocking quickly on I/O are promoted.
*   **CFS (Completely Fair Scheduler).** A preemptive algorithm used by default in the Linux kernel. Instead of rigid quanta, it calculates virtual runtime considering priority weights, aiming to provide each process with a perfectly fair share of CPU time.

Status: Archive / Read-Only. The project is completed, no further development or changes are planned.
