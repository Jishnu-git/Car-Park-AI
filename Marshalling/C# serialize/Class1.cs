using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Threading.Tasks;
using System.Xml.Serialization;
using System.IO;


namespace Serialization_XML
{
    class Class1
    {
        static void main(string[] args)
        {

            try
            {
                XmlSerializer x = new XmlSerializer(typeof(Product));
                StreamReader sr = new StreamReader("product.xml");
                Product p = (Product)x.Deserialize(sr);
                Console.WriteLine("info");
                Console.WriteLine("Forward" + p.forward);
                Console.WriteLine("Backward" + p.backward);
                Console.WriteLine("Right" + p.right);
                Console.WriteLine("Left" + p.left);
                Console.WriteLine("Brakes" + p.brakes);
                Console.WriteLine("Unit" + p.price.unit);
                Console.WriteLine("Value" + p.price.value);
                Console.WriteLine("Discount" + p.price.discount);
                Console.WriteLine("Colour" + p.descryption.color);
                Console.WriteLine("Size" + p.descryption.size);
                Console.WriteLine("weigth" + p.descryption.weigth);

            }
            
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }


        }
            

    }
}
