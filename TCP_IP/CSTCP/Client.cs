using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace CSTCP
{
    public class Client
    {
        private static Socket sock;
        public static void CreateConnection()
        {
            sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPHostEntry ipHost = Dns.GetHostEntry("localhost");
            IPAddress ipaddr = ipHost.AddressList[1];
            IPEndPoint ipe = new IPEndPoint(ipaddr,10020);
            try{
                sock.Connect(ipe);
            }
            catch(ArgumentException ae){
                Console.WriteLine(ae.ToString());
            }
            catch(SocketException se){
                Console.WriteLine(se.ToString());
            }
            catch( Exception e){
                Console.WriteLine(e.ToString());
            }
            //Data to be sent 
        }
        public static void SendMessage(string message){
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(message);
            Console.WriteLine(msg.Length);
            int bytesSent  = sock.Send(msg);
        }
        public static string RecvMessage(){
            byte[] bytes = new byte[1024];
            int bytesRec = sock.Receive(bytes);
            string recvMsg  = Encoding.ASCII.GetString(bytes,0,bytesRec);

            return recvMsg;
        }
    }
}
