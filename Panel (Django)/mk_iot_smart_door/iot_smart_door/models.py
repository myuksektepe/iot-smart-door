from django.db import models


class Doors(models.Model):
    name = models.CharField(max_length=300, verbose_name="Kapı İsmi", unique=True)
    is_visible = models.BooleanField(verbose_name="Aktif Mi?", default=True)
    create_at = models.DateTimeField(verbose_name="Oluşturulma Tarihi", auto_now_add=True)
    updated_at = models.DateTimeField(verbose_name="Güncellenme Tarihi", auto_now=True)

    class Meta:
        verbose_name = "Kapı"
        verbose_name_plural = "01 - Kapılar"

    def __str__(self):
        return self.name


class Personnels(models.Model):
    first_name = models.CharField(max_length=50, verbose_name="İsim", null=True)
    last_name = models.CharField(max_length=50, verbose_name="Soyisim", null=True)
    email = models.CharField(max_length=50, verbose_name="E-mail", null=True, blank=True)
    phone_number = models.CharField(max_length=50, verbose_name="Telefon Numarası", null=True, blank=True)
    is_visible = models.BooleanField(verbose_name="Aktif Mi?", default=True)
    create_at = models.DateTimeField(verbose_name="Oluşturulma Tarihi", auto_now_add=True)
    updated_at = models.DateTimeField(verbose_name="Güncellenme Tarihi", auto_now=True)

    def __str__(self):
        return "%s %s" % (self.first_name, self.last_name)

    class Meta:
        verbose_name = "Kullanıcı"
        verbose_name_plural = "02 - Kullanıcılar"


class Cards(models.Model):
    identity = models.CharField(max_length=100, verbose_name="Kart Kimliği", null=True)
    personnel = models.ForeignKey(Personnels, verbose_name="Kullanıcı", on_delete=models.CASCADE, default="", null=True, blank=True)
    authorized_doors = models.ManyToManyField(Doors, related_name="authorized_doors+", verbose_name="İzinli Kapılar", blank=True)
    unauthorized_doors = models.ManyToManyField(Doors, related_name="unauthorized_doors+", verbose_name="İzinsiz Kapılar", blank=True)
    banned_doors = models.ManyToManyField(Doors, related_name="banned_doors+", verbose_name="Yasaklı Kapılar", blank=True)
    banned = models.BooleanField(verbose_name="Her Yerde Yasakla!", help_text="<b style='color:red'>Dikkat sadece acil durumlarda işaretleyin</b>", default=False)
    is_visible = models.BooleanField(verbose_name="Aktif Mi?", default=True)
    create_at = models.DateTimeField(verbose_name="Oluşturulma Tarihi", auto_now_add=True)
    updated_at = models.DateTimeField(verbose_name="Güncellenme Tarihi", auto_now=True)

    def __str__(self):
        return self.identity

    class Meta:
        verbose_name = "Kart"
        verbose_name_plural = "03 - Kartlar"


class Logs(models.Model):
    door = models.ForeignKey(Doors, verbose_name="Kapı", on_delete=models.CASCADE, null=True, blank=True)
    card = models.ForeignKey(Cards, verbose_name="Kart", on_delete=models.CASCADE, null=True, blank=True)
    personnel = models.ForeignKey(Personnels, verbose_name="Personel", on_delete=models.CASCADE, null=True, blank=True)
    status = models.BooleanField("Geçiş İzni", null=True)
    message = models.CharField("Mesaj", max_length=100, null=True, blank=True)
    reason = models.CharField("Sebep", max_length=100, null=True, blank=True)
    create_at = models.DateTimeField(verbose_name="Oluşturulma Tarihi", auto_now_add=True)

    def __str__(self):
        return "{} - {} ({}) - {}".format(self.door, self.card, self.personnel, self.create_at)

    class Meta:
        verbose_name = "Kayıt"
        verbose_name_plural = "04 - Kayıtlar"
