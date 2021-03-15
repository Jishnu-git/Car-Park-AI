using System;

namespace CSTCP
{
    public class ClientSrc {
        public static void Main(){
            Client.CreateConnection();
            Client.SendMessage("Hello World");
            Console.WriteLine(Client.RecvMessage());
        }
    }
}