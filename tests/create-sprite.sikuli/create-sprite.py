import shutil

try:
    import org.sikuli.script.SikulixForJython
    from sikuli import *
except ImportError:
    print("No run by sikuli jython")

fn= inspect.getsourcefile(lambda:0)
path= os.path.dirname(os.path.abspath(fn))
bsfn= os.path.splitext(os.path.basename(fn))[0]
addImagePath(path)

def checkText(string, region):
    # region.highlight(1) # for debugging
    matches= region.collectWords() # list of matches, not words
    words= list(map(lambda x:x.getText(), matches)) # get text from matches https://stackoverflow.com/a/9135180
    matching= [s for s in words if string in s] # https://stackoverflow.com/a/4843172
    if not matching:
        for word in words:
            print("No match found: " + word.encode('utf-8')) # encode in case OCR found odd chars  
        # exit(10)
    else:
        for word in matching:
            print("Match found: " + word.encode('utf-8')) # encode in case OCR found odd chars  

def main():
    wait("ctp2start-scr.png", 100) # still works with sprite button shown
    click("ctp2sprite-test-btn.png")
    wait(10) # progress bar does not appear at once
    if waitVanish(Pattern("ctp2progress-bar.png").similar(0.95), 100): # wait until progressbar vanishes
        wait(10) # some more time
        Settings.TypeDelay = 0.1; # ctp2-SDL needs some more time
        click(Pattern("ctp2sprite-sprite-name-field.png").similar(0.95).targetOffset(0, -10)) # for OCR region
        match= getLastMatch() # for reconstructing an OCR region https://answers.launchpad.net/sikuli/+question/418151
        region= Region(match.getX()+3, match.getY()+3, 70, 17) # regtion for OCR
        print sys.argv[1:]
        for spriteNumber in sys.argv[1:]:
            spriteFName= "GG" + spriteNumber
            print("Processing " + spriteFName)
            click(Pattern("ctp2sprite-sprite-name-field.png").similar(0.7).targetOffset(0, -10)) # text field needs click to get focus
            click(Mouse.at())
            hover(getLastMatch().getTarget().offset(0, -100)) # move mouse out of the text field for later matching
            count= 0
            while not exists(Pattern("ctp2sprite-sprite-name-field.png").similar(0.98), 0.01) and count < 100: # high similarity to ensure empty text field
                type(Key.BACKSPACE) # hit backspace until name-field is empty
                count= count + 1
            type(spriteFName + '.txt') # import from TXT and image series
            checkText(spriteFName + '.tx', region) # excluding last char due to OCR of text-cursor
            click("ctp2sprite-load-btn.png")
            click(Mouse.at())
            wait(10)
            file = capture(SCREEN.getBounds())
            if file:
                shutil.move(file, bsfn + '_.png')
            click(Pattern("ctp2sprite-sprite-name-field.png").similar(0.7).targetOffset(0, -10)) # text field needs click to get focus
            click(Mouse.at())
            hover(getLastMatch().getTarget().offset(0, -100)) # move mouse out of the text field for later matching
            count= 0
            while not exists(Pattern("ctp2sprite-sprite-name-field.png").similar(0.98), 0.01) and count < 100: # high similarity to ensure empty text field
                type(Key.BACKSPACE) # hit backspace until name-field is empty
                count= count + 1
            type(spriteFName + '_.spr') # save to SPR
            checkText(spriteFName + '_.sp', region) # region stays the same
            click("ctp2sprite-save-btn.png")
            # 2nd click causes segfault: click(Mouse.at())
            wait(10)
            file = capture(SCREEN.getBounds())
            if file:
                shutil.move(file, bsfn + '.png')
    else:
        exit(20)
    exit(0)

if __name__ == "__main__":
    main()
