# SoftwareApp
This software application is being created for the Doma Immigration Law firm.

Doma's filing system is organized by client names followed by an 8 character integer number.
The purpose of the 8 character number is to number and orgainze files, as well as to prevent file duplicates in the event of more than one client possessing the same name.

This app creates an 8 character ID made up of integers.
The ID is stored in a shared OneDrive file accessible to all Doma employees.
The application tracks all currently used IDs and provides the user a new available ID that can be unused.
Because the file that tracks all IDs is stored on a shared OneDrive, the application is able to update the file in real-time ensuring multiple unique IDs can be made for multiple employees to use for their client files at the same time.
