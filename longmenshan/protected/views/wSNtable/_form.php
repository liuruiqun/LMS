<?php
/* @var $this WSNtableController */
/* @var $model WSNtable */
/* @var $form CActiveForm */
?>

<div class="form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'id'=>'wsntable-form',
	// Please note: When you enable ajax validation, make sure the corresponding
	// controller action is handling ajax validation correctly.
	// There is a call to performAjaxValidation() commented in generated controller code.
	// See class documentation of CActiveForm for details on this.
	'enableAjaxValidation'=>false,
)); ?>

	<p class="note">Fields with <span class="required">*</span> are required.</p>

	<?php echo $form->errorSummary($model); ?>
    <div class="row">
        <?php echo $form->labelEx($model,'NUMSEQ'); ?>
        <?php echo $form->textField($model,'NUMSEQ',array('size'=>10,'maxlength'=>10)); ?>
        <?php echo $form->error($model,'NUMSEQ'); ?>
    </div>

	<div class="row">
		<?php echo $form->labelEx($model,'NODEID'); ?>
		<?php echo $form->textField($model,'NODEID',array('size'=>10,'maxlength'=>10)); ?>
		<?php echo $form->error($model,'NODEID'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'TYPE'); ?>
		<?php echo $form->textField($model,'TYPE',array('size'=>10,'maxlength'=>10)); ?>
		<?php echo $form->error($model,'TYPE'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'TYPENOTE'); ?>
		<?php echo $form->textField($model,'TYPENOTE',array('size'=>10,'maxlength'=>10)); ?>
		<?php echo $form->error($model,'TYPENOTE'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'VALUE'); ?>
		<?php echo $form->textField($model,'VALUE'); ?>
		<?php echo $form->error($model,'VALUE'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'SENDTIME'); ?>
		<?php echo $form->textField($model,'SENDTIME',array('size'=>20,'maxlength'=>20)); ?>
		<?php echo $form->error($model,'SENDTIME'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'RECVTIME'); ?>
		<?php echo $form->textField($model,'RECVTIME',array('size'=>20,'maxlength'=>20)); ?>
		<?php echo $form->error($model,'RECVTIME'); ?>
	</div>

	<div class="row">
		<?php echo $form->labelEx($model,'LOCALTION'); ?>
		<?php echo $form->textField($model,'LOCALTION',array('size'=>10,'maxlength'=>10)); ?>
		<?php echo $form->error($model,'LOCALTION'); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton($model->isNewRecord ? 'Create' : 'Save'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- form -->