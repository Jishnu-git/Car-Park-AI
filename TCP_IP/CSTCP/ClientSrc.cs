using System;

namespace CSTCP
{
    public class ClientSrc {
        public static void Main(){
            Client.CreateConnection();
            while(true){
                Client.SendMessage("Hello World");
                Console.WriteLine(Client.RecvMessage());
            }

        }
    }
}