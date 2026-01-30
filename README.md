# TSP with Penalty (Prize-Collecting TSP)

---

## 🗂️ Table of Contents
1. [Introduction](#-introduction)
2. [Purpose](#-purpose)
3. [Algorithms Used](#-algorithms-used-in-the-project)
4. [Our Algorithm](#️-our-algorithm-for-tsp-with-penalty)
5. [Ideas Behind](#-ideas-behind-the-algorithm)

---

## 📌 Introduction

The **Traveling Salesman Problem (TSP)** is a well-known NP-Hard optimization problem where a salesman must visit all given cities exactly once and return to the starting city with the **minimum total cost** (typically distance). 

In our project, we explore a variation of this classic problem:  
### **TSP with Penalty (Prize-Collecting TSP)**  
Here, the salesman is allowed to skip visiting some cities, but each skipped city incurs a **penalty cost**. The goal is to find the most cost-efficient route, balancing travel distances and penalties for skipped cities.

---

## 🎯 Purpose

The objective of this project is to develop an **efficient algorithm** that:
- Approximates the optimal solution to the TSP with penalty.
- Produces a result in **feasible runtime**, suitable even for large inputs.

---

## 📚 Algorithms Used in the Project

### 1. Nearest Neighbour Algorithm
- A greedy algorithm that builds a path by always moving to the **nearest unvisited city**.
- Fast and simple, but not always optimal.

### 2. 2-opt Algorithm
- An iterative improvement technique that examines pairs of edges and **swaps them** to reduce total path cost.
- Significantly enhances sub-optimal routes like those from Nearest Neighbour.

---

## ⚙️ Our Algorithm for TSP with Penalty

We combined multiple strategies to create a robust solution:
- **Initial Tour Generation**: Uses Nearest Neighbour to visit all cities.
- **Tour Optimization**: Applies 2-opt to improve this initial path.
- **Penalty Optimization**:
  - A function `compute_penalized_cost` applies **dynamic programming** to optimize the tour.
  - It evaluates when to **skip cities** to reduce combined travel and penalty costs.
  - A **lower triangular matrix** is used for efficient memory storage of distances.
- **Final Optimization**: After penalized cost computation, the tour is again refined using 2-opt.
- **Output**: Final path and statistics are written to an output file.

---

## 💡 Ideas Behind the Algorithm

Our approach is driven by:

### 1. **Dynamic Programming**
- Used in `compute_penalized_cost` to determine the minimum cumulative cost to reach each node.
- The cost function:  
  ``` 
  dp[i] = min(dp[i - j] + distance + (j - 1) * penalty) 
  ```  
  where `j` is the number of skipped cities.

### 2. **Iterative Improvement**
- While not traditional local search, our method evaluates subpath combinations to find globally better paths.
- This is embedded within the dynamic programming process.

### 3. **Problem Reduction-Inspired Thinking**
- We start by solving the basic TSP as a **base case**, then enhance it for the penalized variant.
- This reuse of the simpler problem solution reflects a problem-reduction mindset.

---

## 👥 Authors

- Nuraddin Abbasov  
- Sabri Yildiz  
- Burak Demirer

---

> 🧠 This algorithm demonstrates how blending **classical algorithms**, **heuristics**, and **dynamic programming** can effectively tackle NP-Hard problems with practical efficiency.
