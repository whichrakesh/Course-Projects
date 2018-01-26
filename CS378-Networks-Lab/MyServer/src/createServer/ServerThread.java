package createServer;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.Date;
import java.util.LinkedList;
import java.util.Queue;


public class ServerThread extends Thread {
	private Thread t;
	Socket client_socket;
	private String thread_name;
	private String client_ip;
	private boolean keepAlive = false;
	public ServerThread(String name, Socket socket) {
		thread_name = name;
		client_socket = socket;
		client_ip = client_socket.getInetAddress().toString();
	}
	public void Start(){
		System.out.println("------------ starting ServerThread " + thread_name + " ----------"+ "--"+(new Date()).toString());
		if (t==null){
			t = new Thread(this,thread_name);
			t.start();
		}
	}

	public void run(){
		try{
			client_socket.setSoTimeout(30000);
			int request_count = 0;
			BufferedReader in = new BufferedReader(
					new InputStreamReader(client_socket.getInputStream()));
			String inputLine;
			inputLine = in.readLine();
			ResponseThread thread = new ResponseThread(client_socket);;
			while(inputLine != null){
				System.out.println(inputLine);				
				String[] header = inputLine.split(" ");
				while(inputLine!=null && !inputLine.equals("")){
					inputLine = in.readLine();
					String[] tokens = inputLine.split(" ");					
					if(tokens[0].equals("Connection:")){
						//System.out.println(tokens[1]);
						if(tokens[1].equals("keep-alive"))
								keepAlive = true;
					}
					//System.out.println(inputLine);
				}
				if(header[0].equals("GET")){
					String filename = header[1];
						String user = null;
						request_count++;
						if(filename.endsWith("/")){
							filename = filename.concat("index.html");
						}else if(filename.indexOf('.') == -1){
							filename = filename.concat("/index.html");
						}
						if(filename.startsWith("/~")){
							String[] path = filename.split("/");
							System.out.println(path[1]);
							user = path[1].substring(1);
							filename = filename.substring(user.length()+2); 
							filename = "/home/" + user  +"/public_html" + filename;
							
						} else {
							filename = "/home/rakesh/public_html" + filename;
						}						
					if(!thread.isAlive()) {
						thread.addRequest(filename);
						thread.run();
					} else{
						thread.addRequest(filename);
					} 
					
					//out.flush();
					if(keepAlive){
						client_socket.setKeepAlive(true);
						client_socket.setSoTimeout(10000);
					} else {
						client_socket.close();
						break;						
					}
										
				}
				inputLine = in.readLine();
				//System.out.println(inputLine);
			}
			System.out.println("-------exiting while loop in " + thread_name + "--"+(new Date()).toString());
			CreateServer.decrementConnectionCount(client_ip);
		}catch(SocketTimeoutException st ){
			try {
				CreateServer.decrementConnectionCount(client_ip);
				client_socket.close();
				System.out.println("----------socket timeout. exiting from " + thread_name + "------------"+ "--"+(new Date()).toString());				
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}catch(IOException ie){
			System.out.println(ie);
		}
	}
}
