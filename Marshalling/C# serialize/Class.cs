using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Threading.Tasks;
using System.Xml.Serialization;
using System.IO;

namespace Serialization_XML
{
    class Class
    {
        static void main(string[] args)
        {
            try
            {
                List<Product> lp = new List<Product>();
                lp.add(new Product{
                    forward = true,
                    backward = false,
                    right = false,
                    left = false,
                    brakes = true,
                    p = new price { unit = "Rupees", value = 20, discount = 0.20 },
                    d = new descryption { color = "Green", size = 2, weigth = "1kg" }
                });
                lp.add(new Product{
                    forward = true,
                    backward = false,
                    right = false,
                    left = false,
                    brakes = true,
                    p = new price { unit = "Rupees", value = 20, discount = 0.20 },
                    d = new descryption { color = "Green", size = 2, weigth = "1kg" }
                });
                lp.add(new Product{
                    forward = true,
                    backward = false,
                    right = false,
                    left = false,
                    brakes = true,
                    p = new price { unit = "Rupees", value = 20, discount = 0.20 },
                    d = new descryption { color = "Green", size = 2, weigth = "1kg" }
                });
                XmlSerializer x = new XmlSerializer(typeof(Product));
                StreamWriter sw = new StreamWriter("product.xml");
                x.Serialize(sw, lp);
                sw.Close();
            }

            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            Console.ReadLine();
        }
    }
}
