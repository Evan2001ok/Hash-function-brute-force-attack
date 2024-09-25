First is the one-way test

First, I set an initial message, calculate its hash value and take its first 24 bits, then use rand to simulate a brute force attack to find another message with the same hash value as the initial message. When two different messages with the same hash are found, the attack is considered successful, and the number of attempts required for the attack is recorded.

Next is the collision-free property test

First, I generate a random message and calculate the hash value, then record its hash value and store the hash value in an empty hash table. Then use collision detection. When a collision occurs, the attack is successful, and the number of attempts required for the attack is recorded.
