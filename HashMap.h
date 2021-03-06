#pragma once

#include <string>  //for std::string type
#include <utility> //for std::pair type for key, value pairs
#include <list>    //for std::list type for array of lists (chaining)

//HashMap class declaration-----------------------------------------------------
template<class T>
class HashMap {
public:
  HashMap();
  HashMap(const int size);
  ~HashMap();

  bool set(const std::string key, T value);
  T get(const std::string key) const;
  T remove(const std::string key);
  float load() const;
private:
  std::list<std::pair<std::string, T> >* map; //pointer to list of key-value pairs
  unsigned int itemsInMap, mapSize;           //used for load function
  unsigned long hash(const std::string key) const;
};

//Function Definitions----------------------------------------------------------
//constructors-------------------------------
template<class T>
HashMap<T>::HashMap() : HashMap(10) {/*body inentionally left empty*/}

template<class T>
HashMap<T>::HashMap(const int size) {
  map = new std::list<std::pair<std::string, T> >[size]; //create dynamic array of lists of given size
  mapSize = size;
  itemsInMap = 0;
}
//destructor-------------------------------
template<class T>
HashMap<T>::~HashMap() {
  delete[] map;   //deallocate memory used by array of lists
  map = NULL;     //set pointer to NULL
}

//data operation functions-------------------------------
/*
stores the key-value pair <key, value> inside map at a position determined by the hash function
@Precondition key is not empty and map does not already contain a value for given key
@Postcondition map contains the new value or it remains unchanged
@parameters the key for the object; the object to store
@return True if store is successful or false otherwise
*/
template<class T>
bool HashMap<T>::set(const std::string key, T value) {
  //if key is blank, return
  if (key == "")
    return false;

  std::list<std::pair<std::string, T> >* listAtKey;
  //Function Call----------------------------------
  int insertIndex = hash(key), startIndex = insertIndex;

  int smallestListSize = 32767, smallestListIndex = insertIndex;
  int increment = 0, rehashAttempts = -1;
  //use quadratic probing to increment through positions in map array if the
  //list at the position generated by the hash isn't empty. Do this until an empty
  //list is found or five open addressing attempts are made
  do {
    //increment position in map array
    insertIndex = (startIndex + increment * increment) % mapSize;
    listAtKey = &map[insertIndex];

    //check all items in current list
    for (typename std::list<std::pair<std::string, T> >::iterator itPair = listAtKey->begin(); itPair != listAtKey->end(); ++itPair) {
      if (itPair->first == key) {
        if (itPair->second != T())
          return false;   //return false if a key, value pair already exists and is not empty
        itPair->second = value;
        return true;      //if the value is empty, replace it with the new value and return true
      }
    }

    //store the index of the smallest list to then store the object in that list
    //in case an empty slot cannot be found after 5 open addressing attempts
    int listSize = listAtKey->size();
    if (listSize <= smallestListSize) {
      smallestListSize = listSize;
      smallestListIndex = insertIndex;
    }

    increment++;
    rehashAttempts++;
  } while (listAtKey->size() > 0 && rehashAttempts < 5);

  //only increment the number of items in map if the object is stored at an empty position
  bool incrementNumberOfItems = true;
  if (listAtKey->size() > 0) {
    incrementNumberOfItems = false;
    insertIndex = smallestListIndex;
  }

  listAtKey->push_back(std::pair<std::string, T>(key, value));
  if (incrementNumberOfItems)
    itemsInMap++;
  return true;
}

/*
gets the value stored in map at the position corresponding to the given key
@Precondition key is not empty
@Postcondition
@parameters the key for the object
@return the value for the given key or the default value for the type of objects stored in map
*/
template<class T>
T HashMap<T>::get(const std::string key) const {
  //return the default value for the type of objects being stored if the key is blank (cannot return NULL without implicitly casting all types to it)
  if (key == "")
    return T();

  std::list<std::pair<std::string, T> > listAtKey;
  int getIndex = hash(key), startIndex = getIndex;

  //go through the six possible lists starting from the list at the position generated by the hash
  //and incrementing with quadratic probing just like in set
  int increment = 0;
  for (int i = 0; i <= 6; i++) {
    getIndex = (startIndex + increment * increment) % mapSize;
    listAtKey = map[getIndex];
    //go through the current list and if a pair is found with the same key, return the value
    for (typename std::list<std::pair<std::string, T> >::const_iterator itPair = listAtKey.begin(); itPair != listAtKey.end(); ++itPair) {
      if (itPair->first == key)
        return itPair->second;
    }
    increment++;
  }
  return T();  //return the default object if no match is found
}

/*
removes the value stored in map at the position corresponding to the given key, but DOES NOT decrease the load because only the value is erased (from instructions)
@Precondition key is not empty
@Postcondition value of pair in map that needed to be removed is set to its default or map remains unchanged
@parameters the key for the object
@return the value for the given key or the default value for the type of objects stored in map
*/
template<class T>
T HashMap<T>::remove(const std::string key) {
  if (key == "")
    return T();

  std::list<std::pair<std::string, T> >* listAtKey;
  int removeIndex = hash(key), startIndex = removeIndex;

  //go through the six possible lists starting from the list at the position generated by the hash
  //and incrementing with quadratic probing just like in set
  int increment = 0;
  for (int i = 0; i <= 6; i++) {
    removeIndex = (startIndex + increment * increment) % mapSize;
    listAtKey = &map[removeIndex];   //store pointer to list in map, not copy
    //go through the current list and if a pair is found with the same key...
    for (typename std::list<std::pair<std::string, T> >::iterator itPair = listAtKey->begin(); itPair != listAtKey->end(); ++itPair) {
      if (itPair->first == key) {
        T value = itPair->second;  //set a temporary variable to the value of the value part of the pair
        itPair->second = T();      //set the value part of the pair to its default

        return value;
      }
    }
    increment++;
  }
  return T();
}

/*
calculates the load factor of the map
@Precondition
@Postcondition
@parameters
@return load factor of map. since chaining is used itemsInMap is the number of not empty lists in map, not all the items in all the lists
*/
template<class T>
float HashMap<T>::load() const {
  return float(itemsInMap)/float(mapSize);
}

/*
generates the index in map to store object at
@Precondition
@Postcondition
@parameters the key for the object
@return index in map to store object at
*/
template<class T>
unsigned long HashMap<T>::hash(const std::string key) const {
  unsigned long sum = 0;
  //for the number of letters in key...
  for (int i = 0; i < key.length(); i++) {
    int multiplier = (i+1)*(i+1);  //multiply each letter by perfect squares
    sum += int(key[i]) * multiplier; //add the result to the sum
  }
  return sum % mapSize;  //modulus to fit into size of map
}
