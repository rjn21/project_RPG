# project_RPG

- countRooms return nrOfRooms + 1
- Segmentation fault in line 537 (don't know why) in function countRoomsRek(...)
  (sum += countRoomsRek(roomInDiretion, wasIn);)
  --> occurs always in the last room the algorithm "looks at" (in this case in room 13)
