using System.Runtime.CompilerServices;
namespace MakeCMake;

static class Program
{
	static void mainImpl()
	{
		string root = findCommonRoot();
		generate( root, "FFmpegStatic", "FFmpegStatic.vcxproj" );
		generate( root, "ResamplerStatic", "ResamplerStatic.vcxproj" );
		generate( root, "AudioCodecs", "AudioCodecs.vcxproj" );
	}

	static void generate( params string[] components )
	{
		string path = Path.Combine( components );
		Generator.generate( path );
	}

	static int Main( string[] args )
	{
		try
		{
			mainImpl();
			return 0;
		}
		catch( Exception ex )
		{
			Console.Error.WriteLine( ex.Message.Trim() );
			return 1;
		}
	}

	/// <summary>Construct path to the <c>$(ProjectDir)/../</c> folder</summary>
	static string findCommonRoot( [CallerFilePath] string? thisSource = null )
	{
		string? dir = thisSource;
		for( int i = 0; i < 2; i++ )
			dir = Path.GetDirectoryName( dir );
		if( Directory.Exists( dir ) )
			return dir;
		throw new FileNotFoundException();
	}
}