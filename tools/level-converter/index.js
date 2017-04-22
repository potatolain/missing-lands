// Very quick, very dirty level creator.
// Will turn a json file from tiled into a header and data file, suitable for inclusion in the project.
var path = require('path'),
	fs = require('fs'),
	file = null,
	SCREEN_WIDTH = 16,
	SCREEN_HEIGHT = 12,
	SCREEN_HEIGHT_PADDED = 16,
	isVerbose = false
	outFile = null,
	name = null,
	outHeader = "",
	levelInfo = "";


var outputString = "";

for (var i = 2; i < process.argv.length; i++) {
	var arg = process.argv[i];
	switch (arg) {
		case '-v':
		case '--verbose':
			isVerbose = true;
			break;
		case '-h':
		case '--help':
			// TODO
			console.info("SORRY! NOPE!");
			process.exit(0);
			break;
		default:
			if (arg[0] == '-') {
				error('Unrecognized option "' + arg + '" ignored.');
			} else if (file !== null) {
				outFile = arg;
				outHeader = outFile.replace('.asm', '.h');
				name = outfile.replace('.asm', '');
			} else {
				file = arg;
				lvlName = path.basename(arg, '.json');
			}
			break;
	}
}

function verbose() {
	if (isVerbose) {
		console.info.apply(this, arguments);
	}
}

function out() {
	console.info.apply(this, arguments);
}

function error() {
	console.error.apply(this, arguments);
}

if (file == null) {
	console.info('No file specified');
	process.exit(1);
}

// Attempt to guess what file to put out to.
if (outFile === null) {
	var inFile = path.join(process.cwd(), file),
		fileDir = path.join(path.dirname(inFile), 'processed'),
		outFile = path.join(fileDir, path.basename(inFile, '.json') + '_tiles.asm');
		outHeader = path.join(fileDir, path.basename(inFile, '.json') + '_tiles.h');
		name = path.basename(inFile, '.json');

	if (!fs.existsSync(fileDir)) {
		verbose('Making directory for output: "' + fileDir + '"');
		fs.mkDir(fileDir);
	}
}

verbose('Processing file "' + file + '". Outputting to: "' + outFile + '"/"' + outHeader+'"');

try {
	levelInfo = require(path.join(process.cwd(), file));
} catch (e) {
	error('Failed loading level from "' + file + '"', e);
	process.exit(1);
}

var width = levelInfo.width,
	height = levelInfo.height,
	roomsWide = width / SCREEN_WIDTH,
	roomsTall = height / SCREEN_HEIGHT,
	data = levelInfo.layers[0].data,
	spriteData = levelInfo.layers[2].data,
	originalSize = data.length,
	rawColumns = [],
	columnDefinitions = [],
	mapColumns = [],
	fileData = '';

verbose('Map width: ' + width + ' height: ' + height + ' length: ' + (width * height) + ' bytes.');

// TODO: This length is technically wrong. (Since we pad the height) Not like it really matters, but...
var headerFileContent = "extern const char " + name + "["+width*height+"];";

var fileContent = ".export _"+name+"\n_"+name+":\n";

for (var x = 0; x < roomsWide; x++) {
	for (var y = 0; y < roomsTall; y++) {
		var roomSpriteData = [];
		fileContent += "\n\n";
		if (x == 7 && y == 7) {
			// HACK ATTACK
			fileContent += "; No last room because rom space";
		} else {

			for (var yy = 0; yy < SCREEN_HEIGHT; yy++) {
				fileContent += ".byte ";
				for (var xx = 0; xx < SCREEN_WIDTH; xx++) {
					// Dizzy yet?
					if (xx != 0) {
						fileContent += ", ";
					}
					var pos = (x * SCREEN_WIDTH) + (y * (roomsWide*SCREEN_WIDTH)*SCREEN_HEIGHT) + (yy * width) + xx;
					fileContent += data[pos] - 1;
					if (spriteData[pos] != 0) {
						roomSpriteData.push(yy*SCREEN_WIDTH+xx);
						roomSpriteData.push(spriteData[pos]-257);
					}
					
				}
				fileContent += "\n";
			}
			while (roomSpriteData.length < 32)
				roomSpriteData.push(255);
			
			fileContent += "; Sprites\n.byte ";
			for (var i = 0; i < 15; i++)
				fileContent += roomSpriteData[i] + ', ';
			fileContent += roomSpriteData[15] + "\n.byte ";
			for (var i = 16; i < 31; i++)
				fileContent += roomSpriteData[i] + ', ';
			fileContent += roomSpriteData[31] + "\n; End sprite data\n";
			for (var yy = SCREEN_HEIGHT+2; yy < SCREEN_HEIGHT_PADDED; yy++) {
				fileContent += ".byte 0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0\n";
			}
		}

	}
}

fs.writeFileSync(outHeader, headerFileContent);
fs.writeFileSync(outFile, fileContent);