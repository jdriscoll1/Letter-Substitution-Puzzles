/*
Name: Jordan D. 
Description: This starts off with a list of words, and changes it to a list of numbers in order from most to least connections
Date: 4/20/21
*/
import java.util.Scanner; 
import java.util.ArrayList; 
import java.io.*; 

public class IntegerDocumentProducer{
   
   public static int numLetters = 4; 
   
   public static void main(String[] args) throws FileNotFoundException, IOException {
      String filename; 
      if(numLetters == 4){filename = "Four_Letters.txt";}
      if(numLetters == 3){filename = "Three_Letters.txt";}
      if(numLetters == 2){filename = "Two_Letters.txt";}
      else{filename = "Four_Letters.txt";} 
      File file = new File(filename);  
      //First, take the word array, and make an arraylist of an arraylist of strings
      ArrayList wordConnections = Create_WordConnectionsList(file); 
      //Then, sort that arraylist by longest 2nd dimension
      sort(wordConnections);
      //Then take from that first dimension and put it into an array
      String[] words =  Convert_ArrayList_StringArray(wordConnections);
      //Then do the array list of array list of strings but make it numbers and exactly which one 
      Convert_StrArr_IntDocument(words); 
      
      
      
      
   }
   
   public static ArrayList<ArrayList<String>> Create_WordConnectionsList(File file) throws FileNotFoundException{
      //Loop through the file in 2 dimensions, and print out the word and its connections
      ArrayList<ArrayList<String>> wordConnections = new ArrayList<ArrayList<String>>();  
      //This is the word who is being compared to 
      String base; 
      //This is the word that may be a connection to the base
      String connection;
      //The scan that controls the base word
      Scanner baseScan = new Scanner(file); 
      //The scan that controls the connection word 
      Scanner connectionScan; 
      
      int baseIndex = 0; 
      while(baseScan.hasNext()){
         //Gets the next word
         wordConnections.add(new ArrayList<String>()); 
        
         base = baseScan.nextLine();
         
          wordConnections.get(baseIndex).add(base); 
         
      
         
         int connectionIndex = 0; 
         //Goes through the file looking for words
         connectionScan = new Scanner(file);  
         while(connectionScan.hasNext()){
            
            connection = connectionScan.nextLine(); 
            if(compare(base, connection)){
           
               wordConnections.get(baseIndex).add(connection); 
            }
            connectionIndex++; 
            
         }
         baseIndex++; 
   
         connectionScan.close();
      
      }
      baseScan.close();
      
      
      return wordConnections; 
   
   
   }
   //Checks to see if string
   public static boolean compare (String s1, String s2) {
      //The index
      int i;
      
      //The number of equal letters
      int equalCount = 0; 
      
      //Goes through each letter to see how many they have in commmon
      for(i = 0; i < numLetters; i++){
         if(s1.charAt(i) == s2.charAt(i)){
            equalCount++; 
         }
      
      }  
      //Returns the number of letters that are in common
      return equalCount == (numLetters - 1); 
   }
   
   public static void Print_ArrayList(ArrayList<ArrayList<String>> wordArrArrList){
      for(ArrayList<String> wordArrList : wordArrArrList){ 
         int i = 0; 
         for(String word : wordArrList){
            
            if(i == 0){
               System.out.printf("%s: ", word);
            }
            else{System.out.printf("%s ", word);} 
            i = 1; 
         
         }
         System.out.println();
      
      }
   
   
   }
   public static void sort(ArrayList<ArrayList<String>> wordArrArrList) {
        int n = wordArrArrList.size();
        for (int i = 1; i < n; ++i) {
   
            ArrayList<String> key = wordArrArrList.get(i);
            int j = i - 1;
 
            /* Move elements of arr[0..i-1], that are
               greater than key, to one position ahead
               of their current position */
            while (j >= 0 && wordArrArrList.get(j).size() < key.size()) {
                wordArrArrList.set(j + 1, wordArrArrList.get(j));
                j = j - 1;
            }

            wordArrArrList.set(j + 1, key);
      
        }
        
    }
    public static String[] Convert_ArrayList_StringArray(ArrayList<ArrayList<String>> wordArrArrList){
      int n = wordArrArrList.size(); 
      String[] words = new String[n];
      int i = 0;  
      for(ArrayList<String> wordArrList : wordArrArrList){
       
         
         words[i] = wordArrList.get(0); 
         i++; 
         
        
      }
      return words; 
    
    
    
    }
    public static void Print_Array(String[] wordArr){
      for(String word : wordArr){
         System.out.println(word + " ");
      }
      
    }
   
   public static void Convert_StrArr_IntDocument(String[] words) throws IOException {
      FileWriter file = new FileWriter("Four_Connections_Int.txt"); 
      String base; 
      String connection; 
      
      int i = 0;
      int j = 0; 
      file.write(words.length + "\n");
      for(i = 0; i < words.length; i++){
         base = words[i]; 
         file.write(base);
         System.out.printf("%s", base); 
         for(j = 0; j < words.length; j++){
            connection = words[j]; 
            if(compare(base, connection)){
               file.write(" " + j); 
               System.out.printf(" %d", j);
            
            }   
         
         }
         file.write("\n");
         System.out.println();
      
      }    
      file.close();
   
   }
}