using System.Xml;
using System.Xml.Linq;
using System.Xml.XPath;
namespace MakeCMake;

/// <summary>List of source files parsed from visual studio project file</summary>
sealed class Sources
{
	/// <summary>C and C++ sources</summary>
	public List<string> cpp { get; }

	/// <summary>NASM assembly sources</summary>
	/// <seealso href="https://www.nasm.us/" />
	public List<string> nasm { get; }

	/// <summary>Parse visual studio project file</summary>
	public static Sources parseProject( string path )
	{
		XDocument doc = XDocument.Load( path );
		XmlNamespaceManager namespaceManager = new XmlNamespaceManager( new NameTable() );
		namespaceManager.AddNamespace( "ns", "http://schemas.microsoft.com/developer/msbuild/2003" );
		string relBase = Path.GetDirectoryName( path )!;

		IEnumerable<string> selectSources( string xpath )
		{
			foreach( XElement elt in doc.XPathSelectElements( xpath, namespaceManager ) )
			{
				string include = ( elt.Attribute( "Include" )?.Value ) ?? throw new ApplicationException( "Missing attribute" );
				string source = include.forwardSlashes();
				if( !Path.IsPathRooted( source ) )
					source = Path.Combine( relBase, source );

				if( !File.Exists( source ) )
					throw new FileNotFoundException( "Source file is missing: " + include );
				yield return source.forwardSlashes();
			}
		}

		List<string> cpp = selectSources( "//ns:ClCompile[@Include]" ).ToList();
		List<string> nasm = selectSources( "//ns:NASM[@Include]" ).ToList();

		cpp.Sort( StringComparer.OrdinalIgnoreCase );
		nasm.Sort( StringComparer.OrdinalIgnoreCase );
		return new Sources( cpp, nasm );
	}

	Sources( List<string> cpp, List<string> nasm )
	{
		this.cpp = cpp;
		this.nasm = nasm;
	}
}