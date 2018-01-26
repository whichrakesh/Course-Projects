package createServer;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;


public class CreateServer {
	static HashMap<String,Integer> connectionsHashMap = new HashMap<String,Integer>();	
	public static void main(String[] args) throws IOException  {
		// TODO Auto-generated method stub			
			ServerSocket server_socket = new ServerSocket(8080);
			Socket client_socket = null;
			int count = 0;
		while(true){
			try {
				String thread_name = "Thread" + count;
				client_socket = server_socket.accept();
				String client_ip = client_socket.getInetAddress().toString();
				if(connectionsHashMap.containsKey(client_ip)){
					int connections_count = connectionsHashMap.get(client_ip).intValue();
					if( connections_count > 20){
						System.err.println("~~~~~~~~~~~~~~~DOS. Maximum request exceeded.~~~~~~~~~~~~~~~~~");
						continue;
					} else {
						connectionsHashMap.put(client_ip,connections_count+1);
					}
				} else {
					connectionsHashMap.put(client_ip,1);
				}
				//System.out.println("starting new connection");
				ServerThread thread = new ServerThread(thread_name, client_socket);
				thread.Start();
				count++;
			}catch (IOException e) {
				// TODO: handle exception
				System.out.println(e);					
			}				
		}
	}
	public static int getConnectionCount(String ip){
		return connectionsHashMap.get(ip).intValue();
	}
	public static void decrementConnectionCount(String ip){
		int connections_count = connectionsHashMap.get(ip).intValue();
		connectionsHashMap.put(ip,connections_count-1);
	}
}
