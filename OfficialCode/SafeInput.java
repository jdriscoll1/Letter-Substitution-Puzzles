import java.util.Scanner; 

/*This is the class that will Guarentee Safe Input and output*/
public class SafeInput{
   
   public static void main(String[] args){
      SafeInput in = new SafeInput(); 
      in.print(); 
      in.game(); 
      String input = in.getInput(); 
   
   
   }
   private native void print();
   private native void game(); 
   static {
         
        String path = System.getProperty("user.dir");
        System.load(path + "/BasicWrapperLib.so"); 
   }
   String getInput(){
      Scanner s = new Scanner(System.in);
      String input = s.nextLine(); 
      s.close(); 
      input = toLowerCase(); 
      return input;  
   
   }




}