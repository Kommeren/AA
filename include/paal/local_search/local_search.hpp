/**
 * @file local_search.hpp
 * @brief Costam
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */

/*! \page local_search Local Search The PAAl.Local Search  Library
Index:
<ul>
    <li> \ref Preliminaries - containing preliminaries to Local Search and our interpretation of it.
    <li> LOCAL SEARCH INTERFACE - containing c++ interfaces of Local Search classes.
    <li> SPECIFIC IMPLEMENTATIONS - containing the general idea of the library.
    <li> 2-opt for TSP  - containing the implementation of the 2_local_search for TSP and basic usage of this class
</ul>


\section Preliminaries 
Let us consider maximization problem	: <br><br>

max f(x) over x in X. <br><br>

A well known heuristic for solving problem formulated in such a way is the local search(LS). <br>
Assume that we have feasible solution x' in X. <br>
The LS algorithm searches the neighborhood N(x') of x' and tries to localize the better solution x'' with f(x'') > f(x'). <br>
If the better solution is found we set x' equal x'' and rerun previous step. <br>
If better solution couldn't be found we finish the search with resulting local optimum x*. <br><br>

This algorithm can be repeated and the best local optimum is presented. <br><br>

Let us write the pseudo code for this operation:  <br><br>

<pre>
 local_search() 
 {
     x -> random_solution(X)  
     for_each(Update u in N(x)) 
     { 
         if(gain(apply u on x) > 0) 
         {
             x -> apply u on x 
         }	
     }
     return x
 }
</pre>

Note that we are working on updates (not on the full solution). This idea is going to be used in the c++ code. <br> 
The reason of this is the fact that usually the updates are much lighter than the full solutions. <br>
<br>
Note that in many cases, eg. k-local search or facility location, our solution is  the collection of the elements.<br>
In many cases for such solutions we search the neighborhood of each solution element and try to improve it by changing some part of the solution near the chosen solution element. <br>
<br>
In this cases it is more convenient to proceed in the following way:<br>

<pre>
 local_search() 
 {
     x -> random_solution(X) 
     for_each(Element e of x)
     {
      for_each(Update u in N(e)) 
      { 
         if(gain(apply u on x) > 0) 
         {
             x -> apply u on x 
         }	
      }
     }
     return x
 }
</pre>
<br>
We find this algorithm schema  extremely useful in our implementation! <br>
We will refer to this schema as LocalSearchMultiSolution. <br>
Also if necessary we will refer to the "normal" LS as LocalSearchSingleSolution.

\section local_search_interface  LOCAL SEARCH INTERFACE 
\subsection local_search_single LOCAL SEARCH SINGLE SOLUTION 

In order to present the LS interface we need to introduce several concepts.<br>
Note that <i>Solution</i> is the solution type and <i>Update</i> is the type of single update. <br>
<i>UpdateIteratorsRange</i> is assumed to be std::pair of iterators, pointing to the begin and end of the updates collection. <br><br>

Concepts:
<ol>
    <li> <i>NeighborhoodGetter</i>  is a concept class responsible for getting the neighborhood of the current solution  
    <pre>
    NeighborhoodGetterArchetype {
        UpdateIteratorsRange get(const Solution & s)
    }
    </pre>
    <li> <i>ImproveChecker</i> is a concept class responsible for checking if the specific update element improve the solution.
    <pre>
    ImproveCheckerArchetype {
        int gain(const Solution & s, const Update & update);
    }
    </pre>
    <li> <i>SolutionUpdater</i> is a concept class responsible for updating the solution with the Update.
    <pre>
    SolutionUdaterArchetype {
        int update(Solution & s, const Update & update);
    }
    </pre>
    
    <li> <i>StopCondition</i> is a concept class responsible for stop condition.
    <pre>
    StopCondition {
        bool stop(const Solution & s, const Update & update);
    }
    </pre>


</ol>

Now we can introduce the paal::local_search::LocalSearchStep interface.


\subsection local_search_multi LOCAL SEARCH MULTI SOLUTION 

The interface and conceptes of the  LocalSearchMultiSolution are ver simmilar to the LocalSearchSingleSolution ones.<br>


<br>
Note that <i>SolutionElement</i> is the type of the specific element of the solution. <br>
<i>SolutionElementIterator</i> is assumed to be the type of iterators over solution. <br><br>

Concepts:
<ol>
    <li> <i>MultiSolution</i>  is a concept class representing the  solution  
    <pre>
    MultiSolutionArchetype {
        SolutionElementIterator begin();
        SolutionElementIterator end();
        InnerSolution get(); // OPTIONAL, very often solution concept is just adapter containing real solution, 
                             // The inner solution type isInnerSolution
    }
    <li> <i>MultiNeighborhoodGetter</i>  is a concept class responisble for getting the neighborhood of the current solution  
    <pre>
    MultiNeighborhoodGetterArchetype {
        UpdateIteratorsRange get(const Solution & s, const SolutionElement &)
    }
    </pre>
    <li> <i>MultiImproveChecker</i> is a concept class responsible for checking if the specific update element improve the solution.
    <pre>
    MultiImproveCheckerArchetype {
        int gain(const Solution & s, const SolutionElement &, const Update & update);
    }
    </pre>
    <li> <i>MultiSolutionUpdater</i> is a concept class responsible for updating the solution with the Update.
    <pre>
    MultiSolutionUdaterArchetype {
        int update(Solution & s, const SolutionElement &, const Update & update);
    }
    </pre>
    
    <li> <i>MultiStopCondition</i> is a concept class responsible for stop condition.
    <pre>
    StopCondition {
        bool stop(const Solution & s, const SolutionElement & se, const Update & update);
    }
    </pre>

</ol>


Now we present the paal::local_search::LocalSearchStepMultiSolution inteface.


</pre>
<br>

\section two_opt 2-opt FOR TSP 

Now we can draw for you interface and the example implementation and usage of 2_local_search for TSP. <br><br>

In order to present 2-opt we need to introduce some concept and types. 

<ol>
<li> VertexType - denotes type of the TSP vertex.
<li> Metric need to have fulfill the interface: 
<pre>
    MetricArchetype {
        DistanceType operator()(VertexType,VertexType);
    }
</pre> 

<li>  Cycle need to have fulfill the interface: 
<pre>
    CycleArchetype {
        void flip(VertexType,VertexType);
        EdgeIteratorRange getEdgesRange();
    }
</pre> 
</ol>

Now we can present implementattion of 2-local-search. <br>
Note that in the main algorithm we had to define only constructors.<br>
The rest is done by base class LocalSearchMultiSolution and the implemented helpers.

\include 2_local_search.hpp

Basic usage of this algorithm is extremely simple and elegant. <br> We are using some helper functions from the library.

\snippet 2_local_search_test.cpp Two Local Search Example

Although the basic usage is very simple, the sophisticated user can still easily change default parameters and exchange them with his ones. <br><br><br><br><br><br>
 
*/
#include "single_solution/local_search_single_solution.hpp"
#include "single_solution/local_search_single_solution_obj_function.hpp"
#include "multi_solution/local_search_multi_solution.hpp"