# PES_Project_6
 
Authors: Mike Fruge and Mark Hinkle

Repo Contents: This repository includes all modules outlined in the Project 6 assignment documentation. For this project, we have added freeRTOS compatability as well as ADC, DAC, and DMA functionality. This repository also contains Oscilloscope captures of both the DAC output for Program 1 and ADC readings for Program 2.Lastly, the repo contains this README document.

Observations and Problems: For this project, one of the most difficult tasks we faced was introducing freeRTOS into it. Fortunately, we were able to figure out how to use the freeRTOS API to do most of the things we needed to. The most difficult problem that we found was introducing our logger into the code after creating most of the program. If there was more time, we would create a logging task with a low priority to handle all the logging operations, but once we thought about this it was already too late. We still have not gotten the logger functionality working at the time of this README being written, but we will make an errata commit once we get the logger working. 

Design Decisions: We decided to keep main very short and have the bulk of our program run in our file app.c, which brings together all the modules and freeRTOS implementation into an application specific file. This was very helpful because we can consolodate our tasks to 1 file, which looks nice and makes for easy debugging. We also decided to create an error handling task that is the highest priority, but is blocked until another task indicates that an error has occured. We decided to use a semaphore for LED access, which was fun to figure out. Lastly, we organized the task priorities so that the most important tasks (Initialization and Error Handling) are a higher priority than the other application tasks so these tasks can be run as soon as they are notified.

Execution Instructions: Anyone who downloads this repository needs only to clean the project, and then rebuild it using the MCUXpresso build options. To debug the code, simply debug the project as PE Micro Probes. TO verify the output of the DAC on an O-scope, probe pin E30.

