import flwgpy

numLetters = 4



#Checks if a word is valid
#1 - Too Long
#2 - Too Short
#3 - Not enough Letters in Common
#4 - Same word as previous
#5 - Not in dictionary 
#6 - Already used 
#0 - just right 
def Validity_Check(prev_id, curr_str, WordToInt_HashMap, IntToWord_HashMap):
    #Verifies the word is not too long
    curr_len = len(curr_str)
    if(curr_len > numLetters):
        return 1
    #Verifies the word is not too short
    elif(curr_len < numLetters):
        return 2

    #Takes the id and converts it to a word 
    #If the previous ID is equal to -1, then it is the first round 
    if(prev_id != -1):
        #The Previous word's ID converted to a string
        prev_str = flwgpy.Convert_IntToWord(prev_id, IntToWord_HashMap)
        equal_letters = 0
        #Loop throguh all the letters
        for prev_letter, curr_letter in zip(prev_str, curr_str):
            if(prev_letter == curr_letter):
                equal_letters += 1
        if(equal_letters < numLetters - 1):
            return 3
        elif(equal_letters >= numLetters):
            return 4 
    #Check if it is a real word
    curr_id = flwgpy.Convert_WordToInt(curr_str, WordToInt_HashMap)
    if(curr_id == -1):
        return 5
    if(flwgpy.getAlgFound(curr_id, IntToWord_HashMap) == 1): 
        return 6
    #Check if the word has been used yet 
    return 0

        
def Take_Input(prev_id, WordToInt_HashMap, IntToWord_HashMap): 
    validity = -1
    user_input = ""
    while(validity != 0):
        #Takes the user input
        user_input = raw_input("Please Enter Word: ")
    
        #If user wants to quit
        if(user_input == "gg"):
            print("Good Game!")
            return -1
        #Checks the validity of a word 
        validity = Validity_Check(prev_id, user_input, WordToInt_HashMap, IntToWord_HashMap)
    
    #Converts the word to an id 
    input_id = flwgpy.Convert_WordToInt(user_input, WordToInt_HashMap)
    
    #Returns the input id 
    return input_id



def Create_HashMaps():
    #The path to the document
    path = "docs/4.txt" 
    #Creates the hash map taht can find a word's ID
    WordToInt_HashMap = flwgpy.Allocate_WordToInt()
    #Creates teh hash map that can find an integer's word 
    IntToWord_HashMap = flwgpy.Allocate_IntToWordStruct()
    #Fills both of the hash maps
    flwgpy.Initialize_HashMaps(WordToInt_HashMap, IntToWord_HashMap, path)
    #Returns both of the hash maps
    return IntToWord_HashMap, WordToInt_HashMap


def FLWG_Python(IntToWord_HashMap, WordToInt_HashMap):
    word_id = flwgpy.ChooseStart(IntToWord_HashMap)
    depth = 15
    flwgpy.setAlgFound(word_id, IntToWord_HashMap)
    print(flwgpy.Convert_IntToWord(word_id, IntToWord_HashMap))
    participant = 0
    winner = -1
    while(winner == -1):
        if(participant % 2 == 0):
            word_id = Take_Input(word_id, WordToInt_HashMap, IntToWord_HashMap)
        else:
            word_id = flwgpy.botPly(word_id, depth, IntToWord_HashMap, flwgpy.minimax)
            if(word_id != -1):
                print(flwgpy.Convert_IntToWord(word_id, IntToWord_HashMap))


        participant += 1
        if(word_id == -1):
            winner = participant % 2
    if(winner == 1):
        print("Bot Wins")
    else:
        print("User Wins")


def main():
    #Starts srand(time(0))
    flwgpy.InstantiateRandomizer()
    
    #Creates the HashMaps
    IntToWord_HashMap, WordToInt_HashMap = Create_HashMaps()
   

    FLWG_Python(IntToWord_HashMap, WordToInt_HashMap)
    
    
    
    #Frees the HashMaps
    flwgpy.Free_HashMaps(WordToInt_HashMap, IntToWord_HashMap)

main()
