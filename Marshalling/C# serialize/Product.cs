using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Threading.Tasks;
using System.Xml.Serialization;
using System.IO;

namespace Serialization_XML
{
    [XmlRoot("UnitTest1")]
    class Product
    {
        [XmlAttribute("id")]
        public string id { get; set; }

        [XmlAttribute("forward")]
        public bool forward { get; set; }

        [XmlAttribute("backward")]
        public bool backward { get; set; }

        [XmlAttribute("right")]
        public bool right { get; set; }

        [XmlAttribute("left")]
        public bool left { get; set; }

        [XmlAttribute("brakes")]
        public bool brakes { get; set; }

        [XmlElement("price")]
        public price p { get; set; }

        [XmlElement("descryption")]
        public descryption d { get; set;}

        [XmlRoot("price")]
        public class price
        {
            [XmlAttribute("unit")]
            public string unit { get; set; }

            [XmlText]
            public int value { get; set; }

            [XmlText]
            public float discount { get; set;}
        }

        [XmlRoot("descryption")]
        public class descryption
        {
            [XmlElement("color")]
            public string color { get; set; }

            [XmlElement("size")]
            public int size { get; set; }

            [XmlElement("weigth")]
            public string weigth { get; set; }
        }
    }
}
