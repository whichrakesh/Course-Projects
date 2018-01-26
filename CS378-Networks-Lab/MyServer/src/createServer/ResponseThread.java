package createServer;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.Socket;
import java.util.LinkedList;
import java.util.Queue;

public class ResponseThread extends Thread {
	Socket client_socket = null;
	Queue<String> request_queue = new LinkedList<String>(); 
	public ResponseThread(Socket socket){
		client_socket = socket;
		System.out.println("****starting new Response thread: " + Thread.currentThread().getId()  +"****");
	}
	public void addRequest(String filename){
		request_queue.add(filename);
	}
	public void run(){
		try{
			DataOutputStream out =
					new DataOutputStream(client_socket.getOutputStream());
			int request_count = 0;
			try{			
				while(request_queue.peek() != null ){
					//System.out.println("The queue at this stage is" + request_queue);
					String filename = request_queue.remove();
					File file = new File(filename);				
					request_count++;
					int num_of_bytes = (int) file.length();					
					FileInputStream inFile = new FileInputStream(filename);
				    byte[] fileInBytes = new byte[num_of_bytes];
				    inFile.read(fileInBytes);
				    inFile.close();
				    System.out.println("sending " + filename + " as response:" + request_count + " in ResponseThread " + Thread.currentThread().getId()  );
					out.writeBytes("HTTP/1.1 200 Document Follows\r\n");				      
					out.writeBytes("Content-Length: " +
						     num_of_bytes + "\r\n");
					out.writeBytes("\r\n");
					out.write(fileInBytes, 0, num_of_bytes);
				}
				System.out.println("*****exiting response thread:" + Thread.currentThread().getId() + "*******" );
			} catch (FileNotFoundException fe){
				System.out.println(fe);
				out.writeBytes("HTTP/1.1 404 Not Found\r\n");
				out.writeBytes("\r\n");
				out.writeBytes("<html><body><center>Error 404: Not Found</center> </body><html>");;
			} 
		} catch(IOException io){
			io.printStackTrace();
		}
	}
}
